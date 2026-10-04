# Compatibilidade de ROM

## Baseline v1.0.0

O loader atual aceita ROMs de 2 KB ate 16 KB.

Mapeamento base usado pela versao final:

- 2/4 KB: sem bankswitch
- 8 KB: F8
- 12 KB: FA
- 16 KB: F6

O nucleo herdado possui codigo para outros esquemas (por exemplo E0/F6SC), mas a deteccao universal ainda nao faz parte desta baseline.

## Consequencia

Tamanho da ROM nao identifica, sozinho, todos os cartuchos Atari 2600. Uma ROM pode baixar corretamente e ainda nao executar se usar um mapper diferente.

Evolucao recomendada: catalogo com metadados explicitos por jogo (URL, tamanho, mapper, regiao e autor), em vez de depender apenas do tamanho.
