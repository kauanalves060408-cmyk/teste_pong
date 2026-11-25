# Pong TUI com bibtela

## Arquivos
- `bibtela.c` / `bibtela.h`: biblioteca da equipe. As mudanças estão marcadas por comentários
  (`[NOVO]`, `[ALTERADO]` e blocos `>>>>>>>>>> INICIO` ... `<<<<<<<<<< FIM`).
- `pong.c`: jogo em arquivo único. Cada implementação nova está entre comentários
  `IMPLEMENTACAO n (INICIO)` e `FIM DA IMPLEMENTACAO n`.
- `Makefile`, `DIARIO_IA.md`

## Implementações no jogo (pong.c)
1. Estado global (modo, cores, estatísticas)
2. Desenho colorido de raquetes e bola
3. Redesenho do campo
4. Computador (adversário no modo 1 jogador)
5. Leitura de teclas por modo
6. Caixa de pontuação (modo 1 jogador)
7. Partida com os dois modos
8. Menu de cores (vermelha, azul, verde)
9. Menu principal e main

## Dependências (Ubuntu/Debian/WSL/Codespaces)
    sudo apt install build-essential libncurses-dev

## Compilar e executar
    make
    ./pong

Terminal com pelo menos 80x25, UTF-8 e cores.

## Controles
- Menu: setas e ENTER (Q volta/sai)
- 1 jogador: W/S ou setas; P mostra a pontuação; Q volta ao menu
- 2 jogadores: W/S (esquerda) e setas (direita)
- Fim de partida: R ou ENTER = nova partida; Q = menu. Vence quem chegar a 5 pontos.
