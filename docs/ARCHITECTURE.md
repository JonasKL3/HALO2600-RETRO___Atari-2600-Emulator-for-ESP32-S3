# Arquitetura

```text
Smartphone / Dabble
        |
        | Bluetooth
        v
+-------------------+
| ESP32-S3 N16R8    |
|                   |
|  Menu / GamePad   |
|  Atari 2600 core  |
+---+----------+-----+
    |          |
    |          +-----------------> PSRAM (framebuffer/buffers)
    |
    +--> LittleFS (/current_game.bin temporario)
    |
    +--> Wi-Fi/HTTPS --> fontes externas de homebrew
    |
    +--> TFT_eSPI --> ILI9341
```

## Fluxo de jogo

1. boot limpa ROM temporaria;
2. menu e exibido;
3. usuario seleciona o jogo;
4. Wi-Fi conecta;
5. ROM e baixada via HTTPS;
6. tamanho/gravacao sao validados;
7. Wi-Fi e desligado;
8. nucleo e inicializado com a ROM;
9. jogo executa;
10. START+SELECT retorna ao menu;
11. ROM temporaria e apagada.
