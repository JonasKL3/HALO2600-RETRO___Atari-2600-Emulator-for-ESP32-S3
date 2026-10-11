# Changelog

## Documentacao - 2026-10-10

- Registrado risco de reset `heap_caps_free` na dependencia DabbleESP32, diagnosticado em outro projeto ESP32-S3.
- Guia de analise, correcao reversivel e script Windows com backup.
- Firmware HALO v1.0.0 e releases inalterados; risco ainda nao reproduzido no HALO.


## v1.0.0

Baseline estavel Atari 2600 para ESP32-S3.

### Adicionado
- ESP32-S3 N16R8
- ILI9341 via TFT_eSPI
- Dabble Bluetooth GamePad
- menu de jogos
- Wi-Fi/HTTPS para ROMs homebrew
- LittleFS temporario
- PSRAM para framebuffer/buffers
- START+SELECT para retorno ao menu

### Corrigido/estabilizado
- orientacao horizontal dos controles
- eixo Y analogico
- prioridade D-pad sobre analógico
- FIRE / INPT4 e comportamento de latch
- heap insuficiente durante TLS
- inicializacao do nucleo apos download
- fluxo de 16 KB/F6
- recuperacao do LittleFS apos troca de particao
- estrutura Arduino e nomes de arquivos
