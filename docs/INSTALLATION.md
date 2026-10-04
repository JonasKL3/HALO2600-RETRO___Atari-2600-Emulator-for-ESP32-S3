# Instalacao

## 1. Arduino IDE

Instale o core ESP32 da Espressif e selecione:

- Board: `ESP32S3 Dev Module`
- Flash Size: `16MB (128Mb)`
- USB CDC On Boot: `Enabled`
- Partition Scheme: `Huge APP (3MB No OTA/1MB SPIFFS)` ou equivalente com APP >= 3 MB e filesystem

## 2. Bibliotecas

Instale:

- TFT_eSPI (Bodmer)
- DabbleESP32 (STEMpedia)

## 3. TFT_eSPI

Configure o ILI9341 e GPIOs conforme `HARDWARE.md`.

## 4. Wi-Fi

Na pasta do firmware:

1. copie `WiFiConfig.example.h`;
2. renomeie a copia para `WiFiConfig.h`;
3. informe SSID e senha.

`WiFiConfig.h` esta no `.gitignore` e nao deve ser publicado.

O firmware tambem compila sem esse arquivo, mas o download de jogos ficara desabilitado ate a configuracao ser criada.

## 5. Abrir e gravar

Abra:

`firmware/HALO_2600_RETRO_ESP32S3/HALO_2600_RETRO_ESP32S3.ino`

Compile, grave e abra o Monitor Serial a 115200.
