# Creditos e reconhecimentos

HALO 2600 RETRO combina trabalho de integracao/adaptacao para ESP32-S3 com codigo de emulacao e bibliotecas open source anteriores.

## Integracao HALO 2600 RETRO

Esta camada do projeto inclui, entre outras adaptacoes:

- integracao ESP32-S3 N16R8;
- menu de jogos na TFT;
- download Wi-Fi/HTTPS sob demanda;
- ROM temporaria em LittleFS;
- framebuffer/uso de PSRAM;
- integracao de controles Dabble;
- retorno ao menu por START+SELECT;
- adaptacoes de compilacao do nucleo para Arduino/ESP32-S3;
- ajustes de entrada, TIA/FIRE, memoria e fluxo de inicializacao.

## x2600 / Virtual 2600

Autor principal historico: **Alex Hornby**, com contribuicoes de outros autores.

Varios arquivos do nucleo presentes neste repositorio carregam cabecalhos originais do x2600 e devem preservar esses avisos.

- Site historico: https://www.hornby.org.uk/v2600
- Creditos historicos: https://www.hornby.org.uk/v2600/people

O site do Virtual 2600 declara que o projeto e copyright de Alex Hornby e outros sob os termos da GNU General Public License.

## TIA Sound

**Ron Fries** - TIA Chip Sound Simulator (`Tiasound.c`).

O proprio cabecalho do arquivo declara TiaSound Copyright (c) 1996 Ron Fries e GNU Library General Public License v2.

## MCUME / espMCUME

**Jean-Marc Harvengt** e colaboradores.

O nucleo/port usado como referencia de partida para o Atari 2600 no ESP32 pertence a linhagem MCUME/espMCUME, que agrega/porta emuladores existentes para microcontroladores.

- https://github.com/Jean-MarcHarvengt/MCUME
- https://github.com/Jean-MarcHarvengt/espMCUME

## DabbleESP32

**STEMpedia** - comunicacao Bluetooth/GamePad.

- https://github.com/STEMpedia/DabbleESP32
- Licenca upstream: GNU LGPL v3

## TFT_eSPI

**Bodmer** e colaboradores - driver/renderizacao TFT.

- https://github.com/Bodmer/TFT_eSPI
- O arquivo `license.txt` upstream documenta partes MIT, BSD/Adafruit e codigo original sob FreeBSD.

## Homebrews / catalogos externos

O firmware pode referenciar fontes externas, entre elas:

- Retrobrews Atari 2600 homebrew collection: https://github.com/retrobrews/atari2600-games
- OpenEmu Update/Homebrew catalog: https://github.com/OpenEmu/OpenEmu-Update

Exemplos de autores identificados pelas fontes usadas:

- Halo 2600 - Ed Fries
- Pac-Man 4K - Dennis Debro
- Anguna - Nathan Tolbert (Moonsweeper)
- Bit Quest / Bit Quest II - Brian W. Shea (Metalbabble)
- DK Arcade 2600 - Byte Knight
- Thrust / Three.s - Thomas Jentzsch

Os jogos e ROMs nao sao obra do HALO 2600 RETRO e permanecem sob os direitos dos respectivos autores.
