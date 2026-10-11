# Reset aleatorio por `heap_caps_free` - Biblioteca DabbleESP32

> **Escopo:** erro real comprovado no SPACE SHOOTER v5.0.0 em ESP32-S3 usando DabbleESP32. O HALO 2600 RETRO tambem utiliza a biblioteca, portanto **pode** apresentar o mesmo defeito nas instalacoes afetadas. Nao foi confirmado um reset no proprio HALO. Esta atualizacao nao modifica o emulador.

## Sintoma

Uma partida ou aplicacao BLE pode reiniciar a placa sem aviso. No Monitor Serial (115200):

```text
assert failed: heap_caps_free heap_caps_base.c:80
(heap != NULL && "free() target pointer is outside heap areas")
Backtrace: ... 0x4200f842 ... 0x4200f95d ... 0x4200fcd5 ...
ELF file SHA256: bf3df46c9
Rebooting...
rst:0xc (RTC_SW_CPU_RST)
```

Esses enderecos e hash sao **do episodio diagnosticado no SPACE SHOOTER**. Os do HALO, se houver, serao diferentes.

## Investigacao e causa provavel

Decodificacao do ELF da mesma compilacao identificou: `operator delete[]` -> `DabbleClass::freeMemoryAllocated()` (`DabbleESP32.cpp:517`) -> `DabbleClass::processInput(int)` (`DabbleESP32.cpp:332`) -> `DabbleClass::processInput()` (`DabbleESP32.cpp:495`) -> `pollDabble()` -> `loop()`.

O parser publico da [DabbleESP32](https://github.com/STEMpedia/DabbleESP32/blob/master/src/DabbleESP32.cpp) possui `arguments = new byte* [argumentnumber];` sem inicializacao. A rotina de liberacao chama `delete[] arguments[i]` para todas as entradas; quando um pacote BLE termina incompleto, alguma posicao ainda nao alocada pode conter ponteiro invalido. A operacao invalida foi confirmada no backtrace; esse mecanismo explica de maneira plausivel o defeito. Uma outra corrupcao de memoria anterior nao pode ser completamente excluida apenas pelo backtrace.

O HALO inclui `DabbleESP32.h` e chama `Dabble.processInput()` no arquivo `firmware/HALO_2600_RETRO_ESP32S3/HALO_2600_RETRO_ESP32S3.ino`, usando a dependencia separada instalada na Arduino IDE.

## Correcao minima

Edite **a biblioteca instalada**, arquivo `DabbleESP32/src/DabbleESP32.cpp`:

```cpp
// ANTES
arguments = new byte* [argumentnumber];
argumentL = new byte [argumentnumber];

// DEPOIS
arguments = new byte*[argumentnumber]();
argumentL = new byte[argumentnumber]();
```

Em C++, os parenteses ao fim das alocacoes inicializam os ponteiros com `nullptr` e os comprimentos com zero. `delete[] nullptr` e permitido. O autor fez esse patch no computador e confirmou que os resets do SPACE SHOOTER pararam. Isso nao garante estabilidade para qualquer outra falha de memoria ou alimentacao.

## Windows - procedimento com backup (Arduino IDE 2)

1. **Feche a Arduino IDE.**
2. Baixe o arquivo [tools/CORRIGIR-DABBLE-ESP32S3.ps1](../tools/CORRIGIR-DABBLE-ESP32S3.ps1) para **Downloads**.
3. Pressione `Win + E`, abra Downloads, clique na barra de enderecos, digite `powershell` e pressione Enter.
4. Execute:

```powershell
powershell -ExecutionPolicy Bypass -File .\CORRIGIR-DABBLE-ESP32S3.ps1
```

5. O script verifica as alocacoes, salva `DabbleESP32.cpp.bak-AAAAMMDD-HHMMSS` e aplica a correcao. Se o arquivo nao estiver na pasta padrao, indique o caminho real:

```powershell
powershell -ExecutionPolicy Bypass -File .\CORRIGIR-DABBLE-ESP32S3.ps1 -LibraryFile 'C:\CAMINHO\DabbleESP32\src\DabbleESP32.cpp'
```

6. **Reabra a Arduino IDE, compile e grave novamente o HALO**; se o firmware ja estava na placa, ele nao recebe automaticamente o patch.
7. Teste uso prolongado do controle, desconexoes e reconexoes, com Monitor Serial em 115200 baud.

**Reverter:** com a Arduino IDE fechada, restaure o `.bak-...` como `DabbleESP32.cpp` e recompile. Uma atualizacao da biblioteca pode sobrescrever o patch. Leia o script antes da execucao; ele aborta se encontrar uma versao nao reconhecida.

## Se houver outro reset

Guarde a mensagem completa, `Backtrace`, `ELF file SHA256` e o `.ino.elf` da **mesma compilacao**. A mensagem `heap_caps_free` nao prova automaticamente que toda falha seja do Dabble. Separar sempre falhas de memoria, watchdog e brownout.

## Referencias

- DabbleESP32 upstream: https://github.com/STEMpedia/DabbleESP32/blob/master/src/DabbleESP32.cpp
- Caso diagnosticado: https://github.com/JonasKL3/SPACE-SHOOTER-ESP32S3/blob/main/docs/DABBLE_BLE_RESET.md
- Espressif heap: https://docs.espressif.com/projects/esp-idf/en/stable/esp32s3/api-reference/system/mem_alloc.html

**Estado:** aviso e ferramenta de correcao publicados como documentacao; nenhuma alteracao no firmware HALO nem na versao v1.0.0.
