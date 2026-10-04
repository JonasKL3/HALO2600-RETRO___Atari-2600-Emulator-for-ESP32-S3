#include <Arduino.h>
#include <TFT_eSPI.h>
#include <LittleFS.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_MODULE
#include <DabbleESP32.h>

#if __has_include("WiFiConfig.h")
  #include "WiFiConfig.h"
#else
  #define WIFI_SSID ""
  #define WIFI_PASSWORD "COLOQUE_SUA_SENHA_AQUI"
#endif

extern "C" {
#include "emuapi.h"
#include "Vcsemu.h"
}

TFT_eSPI tft = TFT_eSPI(320, 240);

// ROM temporaria: sempre apagada ao iniciar e ao sair do jogo.
static const char *TEMP_ROM_PATH = "/current_game.bin";
static const char *OLD_ROM_PATH  = "/halo2600.bin";
static const size_t MAX_ROM_SIZE = 16384;

// Catalogo homebrew distribuido pelo projeto Retrobrews.
// O firmware baixa diretamente do repositorio original quando o usuario escolhe o jogo.
static const char *ROM_BASE_URL =
  "https://raw.githubusercontent.com/retrobrews/atari2600-games/master/";

struct GameEntry {
  const char *name;
  const char *file;      // arquivo no Retrobrews (quando url == nullptr)
  const char *url;       // URL absoluta opcional para outros homebrews
};

static const GameEntry games[] = {
  {"Anguna", "anguna.bin", nullptr},
  {"Bit Quest", "bitquest.bin", nullptr},
  {"Bit Quest II", "bitquest2.bin", nullptr},
  {"DK Arcade 2600", "dkarcade2600.bin", nullptr},
  {"Evil Magician", "emr.bin", nullptr},
  {"Evil Magician II", "emrii.bin", nullptr},
  {"Fishy", "fishy.bin", nullptr},
  {"Flappy The Duck", "flappy_the_duck.bin", nullptr},
  {"Halo 2600", "halo2600.bin", nullptr},
  {"Haunted Bakery", "hauntedbakery.bin", nullptr},
  {"Jammed", "jammed.bin", nullptr},
  {"Kelly Kangaroo", "kellykangaroo.bin", nullptr},
  {"NanoWing", "nanowing.bin", nullptr},
  {"Neko 2600", "neko_2600.bin", nullptr},
  {"PotHole", "pothole.bin", nullptr},
  {"Robo-Ninja Climb", "roboninjaclimb.bin", nullptr},
  {"Runty's Revenge", "runtysrevenge.bin", nullptr},
  {"Sand Castles", "sandcastles.bin", nullptr},
  {"Solar Plexus", "solarplexus.bin", nullptr},
  {"Stardust", "stardust.bin", nullptr},
  {"Three.s", "threes.bin", nullptr},
  {"Thrust", "thrust.bin", nullptr},
  {"Turtle Bay", "turtlebay.bin", nullptr},
  {"Winter Fortress", "winterfortress.bin", nullptr},
  {"Pac-Man 4K", nullptr, "https://raw.githubusercontent.com/OpenEmu/OpenEmu-Update/master/Homebrew/2600/Pac-Man%204K/pacman4K_NTSC.a26"},
  {"Oystron", nullptr, "https://raw.githubusercontent.com/OpenEmu/OpenEmu-Update/master/Homebrew/2600/Oystron/OYSTR29.a26"},
  {"Hunger Shark", nullptr, "https://raw.githubusercontent.com/OpenEmu/OpenEmu-Update/master/Homebrew/2600/Hunger%20Shark/hungershark.0.2.4.a26"}
};

static const int GAME_COUNT = sizeof(games) / sizeof(games[0]);
static const int VISIBLE_ITEMS = 8;

enum AppMode {
  MODE_MENU,
  MODE_GAME
};

static AppMode appMode = MODE_MENU;
static int selectedGame = 0;
static int menuTop = 0;
static int runningGame = -1;
static bool menuNeedsRedraw = true;
static bool emulatorInitialized = false;

// -----------------------------
// Tela / menu
// -----------------------------
static void centerText(const String &txt, int y, uint16_t color, uint8_t size) {
  tft.setTextSize(size);
  tft.setTextColor(color, TFT_BLACK);
  int16_t x = (tft.width() - (int)txt.length() * 6 * size) / 2;
  if (x < 2) x = 2;
  tft.setCursor(x, y);
  tft.print(txt);
}

static void showStatus(const String &line1, const String &line2 = "") {
  tft.fillScreen(TFT_BLACK);
  centerText("HALO RETRO", 30, TFT_CYAN, 3);
  centerText(line1, 95, TFT_WHITE, 2);
  if (line2.length()) centerText(line2, 130, TFT_YELLOW, 1);
}

static void drawMenu() {
  tft.fillScreen(TFT_BLACK);

  tft.fillRect(0, 0, tft.width(), 32, TFT_DARKGREY);
  tft.setTextColor(TFT_CYAN, TFT_DARKGREY);
  tft.setTextSize(2);
  tft.setCursor(8, 8);
  tft.print("HALO RETRO v1.0 - ATARI 2600");

  if (selectedGame < menuTop) menuTop = selectedGame;
  if (selectedGame >= menuTop + VISIBLE_ITEMS) {
    menuTop = selectedGame - VISIBLE_ITEMS + 1;
  }

  const int firstY = 42;
  const int rowH = 21;

  for (int row = 0; row < VISIBLE_ITEMS; ++row) {
    int idx = menuTop + row;
    if (idx >= GAME_COUNT) break;

    int y = firstY + row * rowH;
    bool selected = (idx == selectedGame);

    if (selected) {
      tft.fillRect(4, y - 2, tft.width() - 8, rowH, TFT_BLUE);
      tft.setTextColor(TFT_WHITE, TFT_BLUE);
    } else {
      tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
    }

    tft.setTextSize(1);
    tft.setCursor(10, y + 4);
    tft.print(selected ? "> " : "  ");
    tft.print(games[idx].name);
  }

  tft.drawFastHLine(0, 214, tft.width(), TFT_DARKGREY);
  tft.setTextSize(1);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setCursor(7, 220);
  tft.print("UP/DOWN: escolher   X/SQUARE: jogar");

  menuNeedsRedraw = false;
}

// -----------------------------
// Wi-Fi / download
// -----------------------------
static void wifiOff() {
  // Desconecta sem apagar as configuracoes da interface Wi-Fi.
  // Evita o "wifi:timeout when WiFi un-init" observado nos testes.
  WiFi.disconnect(false, false);
  delay(100);
  WiFi.mode(WIFI_OFF);
  delay(100);
}

static bool connectWiFi() {
  if (strcmp(WIFI_PASSWORD, "COLOQUE_SUA_SENHA_AQUI") == 0 || strlen(WIFI_SSID) == 0) {
    Serial.println("[WIFI] Configure WiFiConfig.h antes de baixar jogos.");
    showStatus("CONFIGURE O WIFI", "Edite WiFiConfig.h");
    delay(1800);
    return false;
  }

  showStatus("CONECTANDO WIFI...", WIFI_SSID);
  Serial.printf("[WIFI] Conectando em %s\n", WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
    Dabble.processInput();
    delay(100);
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[WIFI] Falha na conexao.");
    showStatus("ERRO NO WIFI", "Verifique nome/senha");
    wifiOff();
    delay(1600);
    return false;
  }

  Serial.print("[WIFI] OK. IP: ");
  Serial.println(WiFi.localIP());

  // Da tempo para a pilha TCP/IP e DNS estabilizarem antes do TLS.
  delay(800);

  IPAddress githubIP;
  if (!WiFi.hostByName("raw.githubusercontent.com", githubIP)) {
    Serial.println("[WIFI] DNS falhou para raw.githubusercontent.com");
    showStatus("ERRO DNS", "raw.githubusercontent.com");
    delay(1500);
    wifiOff();
    return false;
  }

  Serial.print("[WIFI] DNS GitHub: ");
  Serial.println(githubIP);
  Serial.printf("[MEM] Heap livre antes HTTPS: %u bytes\n", (unsigned)ESP.getFreeHeap());
  Serial.printf("[MEM] PSRAM livre: %u bytes\n", (unsigned)ESP.getFreePsram());
  return true;
}

static bool downloadGame(int index) {
  if (index < 0 || index >= GAME_COUNT) return false;

  LittleFS.remove(TEMP_ROM_PATH);

  if (!connectWiFi()) return false;

  String url = games[index].url ? String(games[index].url) : (String(ROM_BASE_URL) + games[index].file);
  showStatus("BAIXANDO...", games[index].name);
  Serial.printf("[DOWNLOAD] %s\n", url.c_str());

  WiFiClientSecure client;
  client.setInsecure();
  client.setHandshakeTimeout(30);
  client.setTimeout(30000);

  HTTPClient http;
  // GitHub Raw usa HTTPS. Estes parametros tornam a conexao mais
  // tolerante no ESP32-S3 e evitam problemas com keep-alive.
  http.setReuse(false);
  http.useHTTP10(true);
  http.setConnectTimeout(30000);
  http.setTimeout(30000);
  http.setUserAgent("HALO-RETRO-ESP32S3/1.0");
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

  if (!http.begin(client, url)) {
    Serial.println("[DOWNLOAD] http.begin falhou.");
    showStatus("ERRO DOWNLOAD", "Falha ao abrir URL");
    wifiOff();
    delay(1500);
    return false;
  }

  Serial.println("[HTTPS] Abrindo conexao com GitHub Raw...");
  int code = http.GET();
  if (code != HTTP_CODE_OK) {
    String err = HTTPClient::errorToString(code);
    Serial.printf("[DOWNLOAD] HTTP %d | %s\n", code, err.c_str());
    Serial.printf("[MEM] Heap apos falha HTTPS: %u bytes\n", (unsigned)ESP.getFreeHeap());
    http.end();
    client.stop();
    delay(250);
    wifiOff();
    showStatus("ERRO DOWNLOAD", String("HTTP ") + code);
    delay(1500);
    return false;
  }

  Serial.printf("[HTTPS] Conexao OK. HTTP %d\n", code);

  int announcedSize = http.getSize();
  if (announcedSize > (int)MAX_ROM_SIZE) {
    Serial.printf("[DOWNLOAD] ROM grande demais: %d bytes\n", announcedSize);
    http.end();
    wifiOff();
    showStatus("ROM INCOMPATIVEL", "> 16 KB neste nucleo");
    delay(1800);
    return false;
  }

  fs::File out = LittleFS.open(TEMP_ROM_PATH, FILE_WRITE);
  if (!out) {
    Serial.println("[DOWNLOAD] Nao foi possivel criar ROM temporaria.");
    http.end();
    wifiOff();
    showStatus("ERRO LITTLEFS", "Nao criou arquivo");
    delay(1500);
    return false;
  }

  int written = http.writeToStream(&out);
  out.close();
  http.end();
  client.stop();
  delay(150);
  wifiOff();

  if (written < 0 || !LittleFS.exists(TEMP_ROM_PATH)) {
    LittleFS.remove(TEMP_ROM_PATH);
    Serial.printf("[DOWNLOAD] Falha ao gravar. retorno=%d\n", written);
    showStatus("ERRO DOWNLOAD", "ROM nao gravada");
    delay(1500);
    return false;
  }

  fs::File check = LittleFS.open(TEMP_ROM_PATH, FILE_READ);
  size_t finalSize = check ? check.size() : 0;
  if (check) check.close();

  Serial.printf("[DOWNLOAD] ROM salva: %u bytes\n", (unsigned)finalSize);

  if (announcedSize > 0 && finalSize != (size_t)announcedSize) {
    Serial.printf("[DOWNLOAD] Incompleto: esperado=%d recebido=%u\n",
                  announcedSize, (unsigned)finalSize);
    LittleFS.remove(TEMP_ROM_PATH);
    showStatus("DOWNLOAD INCOMPLETO", "Tente novamente");
    delay(1500);
    return false;
  }

  if (finalSize < 2048 || finalSize > MAX_ROM_SIZE) {
    LittleFS.remove(TEMP_ROM_PATH);
    showStatus("ROM INCOMPATIVEL", String(finalSize) + " bytes");
    delay(1800);
    return false;
  }

  return true;
}

// -----------------------------
// Controle Atari durante o jogo
// -----------------------------
static uint16_t readGameKeys() {
  uint16_t keys = 0;

  // D-pad tem prioridade. Isso evita enviar duas direcoes opostas quando
  // o eixo analogico fica com valor residual enquanto um botao digital e usado.
  const bool dLeft  = GamePad.isLeftPressed();
  const bool dRight = GamePad.isRightPressed();
  const bool dUp    = GamePad.isUpPressed();
  const bool dDown  = GamePad.isDownPressed();

  // O nucleo espvcs usa os nomes LEFT/RIGHT invertidos em relacao aos bits
  // reais do SWCHA. Mantemos a inversao horizontal que ja foi validada.
  if (dLeft != dRight) {
    if (dLeft)  keys |= MASK_JOY2_RIGHT;
    if (dRight) keys |= MASK_JOY2_LEFT;
  } else if (!dLeft && !dRight) {
    const int ax = GamePad.getXaxisData();
    const int DEAD_ZONE = 2;
    if (ax > DEAD_ZONE)       keys |= MASK_JOY2_LEFT;
    else if (ax < -DEAD_ZONE) keys |= MASK_JOY2_RIGHT;
  }

  if (dUp != dDown) {
    if (dUp)   keys |= MASK_JOY2_UP;
    if (dDown) keys |= MASK_JOY2_DOWN;
  } else if (!dUp && !dDown) {
    const int ay = GamePad.getYaxisData();
    const int DEAD_ZONE = 2;

    // Dabble retorna Y positivo para cima neste controle. A versao anterior
    // estava ao contrario no modo analogico.
    if (ay > DEAD_ZONE)       keys |= MASK_JOY2_UP;
    else if (ay < -DEAD_ZONE) keys |= MASK_JOY2_DOWN;
  }

  if (GamePad.isCrossPressed() || GamePad.isSquarePressed()) {
    keys |= MASK_JOY2_BTN;
  }

  // START+SELECT e reservado para sair do jogo.
  // Individualmente, START continua RESET e SELECT continua SELECT.
  bool start = GamePad.isStartPressed();
  bool select = GamePad.isSelectPressed();

  if (!select && (GamePad.isCirclePressed() || start)) {
    keys |= MASK_KEY_USER1;
  }
  if (!start && (GamePad.isTrianglePressed() || select)) {
    keys |= MASK_KEY_USER2;
  }

  return keys;
}

static bool exitComboPressed() {
  static unsigned long comboStart = 0;
  bool both = GamePad.isStartPressed() && GamePad.isSelectPressed();

  if (!both) {
    comboStart = 0;
    return false;
  }

  if (comboStart == 0) comboStart = millis();
  return millis() - comboStart >= 350;
}

static void exitGameToMenu() {
  Serial.println("[MENU] START+SELECT -> saindo do jogo.");
  emu_SetDabbleKeys(0);
  appMode = MODE_MENU;
  runningGame = -1;

  if (LittleFS.exists(TEMP_ROM_PATH)) {
    LittleFS.remove(TEMP_ROM_PATH);
    Serial.println("[MENU] ROM temporaria apagada.");
  }

  // Espera soltar o combo para nao entrar novamente por acidente.
  unsigned long timeout = millis();
  while (millis() - timeout < 1000) {
    Dabble.processInput();
    if (!GamePad.isStartPressed() && !GamePad.isSelectPressed()) break;
    delay(10);
  }

  menuNeedsRedraw = true;
  drawMenu();
}

static void startSelectedGame() {
  int idx = selectedGame;
  if (!downloadGame(idx)) {
    menuNeedsRedraw = true;
    drawMenu();
    return;
  }

  showStatus("CARREGANDO...", games[idx].name);
  Serial.printf("[EMU] Iniciando: %s\n", games[idx].name);

  // O emulador e seus buffers so sao criados depois do download.
  // Isso deixa o maximo de heap interno livre para o handshake TLS.
  if (!emulatorInitialized) {
    Serial.printf("[MEM] Antes de iniciar emulador: heap=%u | PSRAM=%u\n",
                  (unsigned)ESP.getFreeHeap(), (unsigned)ESP.getFreePsram());
    if (!emu_init() || !vcs_Init()) {
      Serial.println("[MEM] Falha ao reservar memoria do emulador.");
      showStatus("ERRO DE MEMORIA", "Reinicie a placa");
      LittleFS.remove(TEMP_ROM_PATH);
      delay(1800);
      menuNeedsRedraw = true;
      drawMenu();
      return;
    }
    emulatorInitialized = true;
    Serial.printf("[MEM] Emulador inicializado: heap=%u | PSRAM=%u\n",
                  (unsigned)ESP.getFreeHeap(), (unsigned)ESP.getFreePsram());
  }

  emu_SetDabbleKeys(0);
  if (!vcs_Start((char *)TEMP_ROM_PATH)) {
    Serial.println("[EMU] Falha ao iniciar ROM.");
    showStatus("ERRO NA ROM", "Falha ao iniciar nucleo");
    LittleFS.remove(TEMP_ROM_PATH);
    delay(1500);
    menuNeedsRedraw = true;
    drawMenu();
    return;
  }

  runningGame = idx;
  appMode = MODE_GAME;

  // Evita que o mesmo X/SQUARE usado para escolher o jogo seja interpretado
  // como FIRE no primeiro frame.
  unsigned long releaseWait = millis();
  while (millis() - releaseWait < 800) {
    Dabble.processInput();
    if (!GamePad.isCrossPressed() && !GamePad.isSquarePressed()) break;
    delay(5);
  }

  emu_SetDabbleKeys(0);
  tft.fillScreen(TFT_BLACK);
}

// -----------------------------
// Controle do menu
// -----------------------------
static void handleMenuInput() {
  static bool prevUp = false;
  static bool prevDown = false;
  static bool prevSelectButton = false;

  bool dUp = GamePad.isUpPressed();
  bool dDown = GamePad.isDownPressed();
  bool up = false;
  bool down = false;

  // No menu o D-pad tambem tem prioridade sobre o analogico.
  if (dUp != dDown) {
    up = dUp;
    down = dDown;
  } else if (!dUp && !dDown) {
    int ay = GamePad.getYaxisData();
    up = ay > 2;
    down = ay < -2;
  }

  bool choose = GamePad.isCrossPressed() || GamePad.isSquarePressed();

  if (up && !prevUp) {
    selectedGame--;
    if (selectedGame < 0) selectedGame = GAME_COUNT - 1;
    menuNeedsRedraw = true;
  }

  if (down && !prevDown) {
    selectedGame++;
    if (selectedGame >= GAME_COUNT) selectedGame = 0;
    menuNeedsRedraw = true;
  }

  if (choose && !prevSelectButton) {
    startSelectedGame();
  }

  prevUp = up;
  prevDown = down;
  prevSelectButton = choose;

  if (menuNeedsRedraw && appMode == MODE_MENU) drawMenu();
}

void setup() {
  Serial.begin(115200);
  delay(400);

  Serial.println();
  Serial.println("========================================");
  Serial.println("HALO RETRO v1.0 FINAL - ATARI 2600");
  Serial.println("========================================");

  tft.init();
  tft.setRotation(2);
  tft.fillScreen(TFT_BLACK);

  // true = se o LittleFS estiver invalido (por exemplo, apos mudar
  // o Partition Scheme), formata automaticamente e monta novamente.
  if (!LittleFS.begin(true)) {
    showStatus("ERRO LITTLEFS");
    Serial.println("[FS] Falha ao montar/formatar LittleFS.");
    while (true) delay(1000);
  }
  Serial.println("[FS] LittleFS montado (formatacao automatica habilitada).");

  // ROMs de jogo nunca sobrevivem a um boot/reset da placa.
  LittleFS.remove(TEMP_ROM_PATH);
  // Limpa a antiga ROM fixa das versoes de desenvolvimento.
  LittleFS.remove(OLD_ROM_PATH);

  Serial.printf("[FS] Total: %u bytes | usado: %u bytes\n",
                (unsigned)LittleFS.totalBytes(),
                (unsigned)LittleFS.usedBytes());

  Dabble.begin("ESP32-S3-HALO-RETRO");
  Serial.println("[DABBLE] Nome BLE: ESP32-S3-HALO-RETRO");

  Serial.printf("[MEM] Menu pronto: heap=%u | PSRAM=%u\n",
                (unsigned)ESP.getFreeHeap(), (unsigned)ESP.getFreePsram());

  Serial.printf("[MENU] %d jogos no catalogo.\n", GAME_COUNT);
  Serial.println("[MENU] UP/DOWN = navegar | CROSS/SQUARE = jogar");
  Serial.println("[JOGO] START+SELECT (350 ms) = voltar ao menu e apagar ROM");

  drawMenu();
}

void loop() {
  Dabble.processInput();

  if (appMode == MODE_MENU) {
    handleMenuInput();
    delay(5);
    return;
  }

  if (exitComboPressed()) {
    exitGameToMenu();
    return;
  }

  emu_SetDabbleKeys(readGameKeys());
  vcs_Step();
}
