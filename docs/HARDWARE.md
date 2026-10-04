# Hardware

## ESP32-S3

- Board Arduino IDE: ESP32S3 Dev Module
- Modulo alvo: ESP32-S3 DEV MODULE-1 / N16R8
- Flash: 16 MB
- PSRAM: 8 MB
- CPU observada: 240 MHz
- USB CDC On Boot: Enabled

## TFT ILI9341

- 2,8 polegadas
- 240 x 320
- SPI

| TFT | ESP32-S3 |
|---|---|
| VCC | 3V3 |
| GND | GND |
| SCK/CLK | GPIO 12 |
| MOSI/SDI | GPIO 11 |
| MISO/SDO | GPIO 13 |
| CS | GPIO 10 |
| DC/RS | GPIO 9 |
| RST | GPIO 4 |
| LED/BL | 3V3 |

Configuracao TFT_eSPI validada:

```cpp
#define ILI9341_DRIVER
#define TFT_MISO 13
#define TFT_MOSI 11
#define TFT_SCLK 12
#define TFT_CS   10
#define TFT_DC   9
#define TFT_RST  4
#define TOUCH_CS 14
#define SPI_FREQUENCY 5000000
#define SPI_READ_FREQUENCY 10000000
#define SPI_TOUCH_FREQUENCY 2500000
#define USE_HSPI_PORT
```

`USE_HSPI_PORT` deve ser mantido conforme a configuracao validada do projeto.

## Touch

O touch XPT2046 da placa testada nao faz parte da baseline v1.0.0. Na unidade investigada, o circuito/controlador esperado nao estava funcional. O projeto atual usa Dabble como entrada.
