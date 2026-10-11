# Instalacao

## 1. Arduino IDE

Instale o core ESP32 da Espressif e selecione:

- Board: `ESP32S3 Dev Module`
- Flash Size: `16MB (128Mb)`
- USB CDC On Boot: `Enabled`
- Partition Scheme: `Huge APP (3MB No OTA/1MB SPIFFS)` ou equivalente com APP >= 3 MB e filesystem

### Ambiente testado

Este projeto foi desenvolvido e validado com:

- Board: ESP32S3 Dev Module
- Arduino ESP32 Core: 3.3.12
- Flash Size: 16MB (128Mb)
- USB CDC On Boot: Enabled
- Partition Scheme: Huge APP (3MB No OTA/1MB SPIFFS)

Versões posteriores do Arduino ESP32 Core podem funcionar, mas ainda não foram validadas oficialmente.
## 2. Bibliotecas

Instale:

- TFT_eSPI (Bodmer)
- DabbleESP32 (STEMpedia)

### Verificacao obrigatoria de estabilidade BLE

A biblioteca DabbleESP32 pode conter um defeito que leva a `assert failed: heap_caps_free` durante o processamento do GamePad. A ocorrencia foi confirmada em outro projeto ESP32-S3, e o HALO usa a mesma biblioteca. Leia [DABBLE_BLE_RESET.md](DABBLE_BLE_RESET.md) para diagnostico, backup, patch e reversao.

O arquivo `tools/CORRIGIR-DABBLE-ESP32S3.ps1` automatiza a correcao na biblioteca instalada, **nao no firmware do HALO**. Depois de aplicar, **recompile e grave**. Atualizacoes da biblioteca podem remover o patch.

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
