# HALO 2600 RETRO

**Emulador Atari 2600 para ESP32-S3 DEV MODULE-1 (N16R8)**

HALO 2600 RETRO e uma plataforma retro experimental/hobby desenvolvida para o ESP32-S3 com 16 MB de Flash e 8 MB de PSRAM. O projeto integra um nucleo Atari 2600 derivado de software open source existente com display ILI9341, controles Bluetooth via Dabble, menu de jogos, download de ROMs homebrew via Wi-Fi/HTTPS, armazenamento temporario em LittleFS e framebuffer em PSRAM.

> Baseline estavel desta etapa: **v1.0.0**.

## Recursos

- ESP32-S3 N16R8
- TFT ILI9341 2,8" 240x320 via SPI
- GamePad Bluetooth pelo aplicativo Dabble
- Menu de jogos na propria tela
- Download HTTPS sob demanda
- LittleFS para ROM temporaria
- ROM apagada ao sair do jogo e no boot/reset
- Framebuffer e buffers grandes em PSRAM
- `START + SELECT` por ~350 ms para voltar ao menu
- Suporte atual a ROMs Atari 2600 de 2 KB a 16 KB, sujeito ao mapper

## Estrutura

```text
HALO-2600-RETRO-ESP32S3/
├── firmware/HALO_2600_RETRO_ESP32S3/   # sketch Arduino e nucleo
├── docs/                                # hardware, instalacao, controles etc.
├── licenses/                            # licencas e avisos de terceiros
├── references/                          # origem dos projetos usados/referenciados
├── CREDITS.md
├── THIRD_PARTY_LICENSES.md
├── CHANGELOG.md
├── LICENSE-NOTICE.md
└── README.md
```

## Instalacao rapida

1. Arduino IDE: selecione **ESP32S3 Dev Module**.
2. Flash Size: **16MB (128Mb)**.
3. USB CDC On Boot: **Enabled**.
4. Partition Scheme: **Huge APP (3MB No OTA/1MB SPIFFS)** ou equivalente com APP >= 3 MB e filesystem.
5. Instale as bibliotecas **TFT_eSPI** e **DabbleESP32**.
6. Configure TFT_eSPI conforme `docs/HARDWARE.md`.
7. Copie `WiFiConfig.example.h` para `WiFiConfig.h` e preencha SSID/senha.
8. Abra `firmware/HALO_2600_RETRO_ESP32S3/HALO_2600_RETRO_ESP32S3.ino`.
9. Compile e grave.
10. Monitor Serial: **115200**.

Leia `docs/INSTALLATION.md` para o passo a passo completo.

## Controles

| Dabble | Atari 2600 / Sistema |
|---|---|
| D-pad / analógico | Movimento |
| CROSS / SQUARE | FIRE |
| CIRCLE / START | RESET do Atari |
| TRIANGLE / SELECT | SELECT do Atari |
| START + SELECT (~350 ms) | Voltar ao menu e apagar ROM temporaria |

O D-pad digital tem prioridade sobre o eixo analógico quando ambos fornecem uma direcao.

## ROMs

Este repositorio **nao distribui ROMs comerciais do Atari 2600**. O firmware referencia fontes externas para alguns jogos homebrew. Os direitos de cada jogo pertencem aos respectivos autores.

A colecao Retrobrews declara que suas ROMs foram aprovadas para distribuicao gratuita **naquele site/projeto apenas**; por isso este repositorio nao deve espelhar essas ROMs. O firmware referencia a hospedagem original. Veja `docs/ROMS_AND_LEGAL.md`.

## Origem do nucleo e bibliotecas

O codigo do nucleo Atari 2600 preserva avisos/copyrights originais de projetos anteriores, incluindo a linhagem **x2600 / Virtual 2600** e adaptacoes usadas no ecossistema **MCUME/espMCUME**. O som TIA inclui codigo de **Ron Fries**. O projeto tambem usa bibliotecas externas, como **DabbleESP32** e **TFT_eSPI**.

Veja:

- `CREDITS.md`
- `THIRD_PARTY_LICENSES.md`
- `licenses/`
- `references/`

## Limites conhecidos da v1.0.0

- ROMs suportadas pelo loader atual: 2 KB a 16 KB.
- Mapeamento automatico base: 2/4K sem bankswitch, 8K F8, 12K FA e 16K F6.
- O nucleo possui codigo para esquemas adicionais, mas a deteccao completa de mappers ainda nao e universal.
- Uma ROM dentro do limite de tamanho pode nao funcionar se utilizar outro esquema de cartucho.
- Audio pode depender das limitacoes do port/nucleo atual.

## Marca

HALO 2600 RETRO e um projeto independente de hobby. Nao e afiliado nem endossado por Atari, Microsoft, Xbox, Halo Studios/343 Industries ou pelos autores/editoras dos jogos citados. Marcas pertencem aos respectivos proprietarios.

## Licenca

Este repositorio contem codigo derivado de varios projetos open source com termos diferentes. **Nao presuma que todo o repositorio esteja sob MIT.** Os avisos originais dos arquivos devem ser mantidos. Leia `LICENSE-NOTICE.md` e `THIRD_PARTY_LICENSES.md` antes de redistribuir.
