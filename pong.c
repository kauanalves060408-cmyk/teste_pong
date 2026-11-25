/*
 * PONG interativo em ANSI C com a biblioteca bibtela (ncurses).
 *
 * Arquivo unico. O jogo base (2 jogadores, bola, raquetes, placar, colisoes
 * e nova partida) e as IMPLEMENTACOES NOVAS estao no mesmo codigo; cada
 * implementacao e delimitada por comentarios assim:
 *
 *     >>> IMPLEMENTACAO n: titulo   (INICIO)
 *     ...
 *     <<< FIM DA IMPLEMENTACAO n
 *
 * Implementacoes: menu (1 ou 2 jogadores), computador no modo 1 jogador,
 * botao de pontuacao (menu e tecla P) e cores (vermelha/azul/verde).
 *
 * Controles:  W/S e setas movem as raquetes; P = pontuacao (1 jogador);
 *             Q = voltar/sair; R ou ENTER = nova partida.
 */
#include "bibtela.h"

#define SCR_H      25
#define SCR_W      80
#define BOX_TOP    1
#define BOX_BOT    24
#define F_TOP      2
#define F_BOT      23
#define F_LEFT     1
#define F_RIGHT    78
#define PADDLE_H   5
#define LP_COL     3
#define RP_COL     76
#define WIN_SCORE  5
#define FRAME_MS   30
#define BALL_EVERY 2

typedef struct {
    int lp, rp;
    int bx, by, dx, dy;
    int sl, sr;
} Jogo;

/* >>>>>>>>>> IMPLEMENTACAO 1: Estado global (modo, cores e estatisticas) (INICIO)
 * Novo: variaveis globais */
static int modo = 1;                    /* 1 = um jogador, 2 = dois jogadores */
static int cor_raquete = XY_PADRAO;     /* cor atual das raquetes             */
static int cor_bola    = XY_PADRAO;     /* cor atual da bola                  */

typedef struct {                        /* estatisticas do modo 1 jogador     */
    int partidas, vitorias, derrotas;
    int melhor_saldo;                   /* maior diferenca de pontos obtida   */
    int ult_sl, ult_sr;                 /* placar da ultima partida           */
} Estat;
static Estat est = {0, 0, 0, 0, 0, 0};
/* <<<<<<<<<< FIM DA IMPLEMENTACAO 1 */

static void desenha_placar(const Jogo *g) {
    xyprintint(0, 2,  "Jogador 1: %d", g->sl);
    xyprint(0, 38, "PONG");
    xyprintint(0, 60, "Jogador 2: %d", g->sr);
}

/* >>>>>>>>>> IMPLEMENTACAO 2: Desenho colorido de raquetes e bola (INICIO)
 * Substituir desenha_raquete() e desenha_bola() da Parte 2 */
static void desenha_raquete(int col, int top) {
    for (int i = 0; i < PADDLE_H; i++)
        xyprintcolor(top + i, col, cor_raquete, "█");
}

static void desenha_bola(const Jogo *g) {
    xyprintcolor(g->by, g->bx, cor_bola, "O");
}
/* <<<<<<<<<< FIM DA IMPLEMENTACAO 2 */

static void apaga_bola(const Jogo *g) { xyprint(g->by, g->bx, " "); }

static void saque(Jogo *g, int dir) {
    g->bx = (F_LEFT + F_RIGHT) / 2;
    g->by = (F_TOP + F_BOT) / 2;
    g->dx = dir;
    g->dy = (rand() % 2) ? 1 : -1;
}

/* >>>>>>>>>> IMPLEMENTACAO 3: Redesenho do campo (INICIO)
 * Novo: substitui o miolo de novo_jogo() (que passa a chamar esta funcao) */
static void redesenha_campo(const Jogo *g) {
    clear_screen();
    xywindow(BOX_TOP, 0, BOX_BOT, SCR_W - 1, 2);
    desenha_placar(g);
    desenha_raquete(LP_COL, g->lp);
    desenha_raquete(RP_COL, g->rp);
    desenha_bola(g);
    if (modo == 1)
        xyprintcenter(SCR_H - 1, SCR_W, "W/S ou setas: mover   P: pontuacao   Q: menu");
    else
        xyprintcenter(SCR_H - 1, SCR_W, "W/S: jogador 1   Setas: jogador 2   Q: menu");
}
/* <<<<<<<<<< FIM DA IMPLEMENTACAO 3 */

static void novo_jogo(Jogo *g) {
    g->lp = g->rp = (F_TOP + F_BOT) / 2 - PADDLE_H / 2;
    g->sl = g->sr = 0;
    saque(g, (rand() % 2) ? 1 : -1);
    redesenha_campo(g);
}

static int move_bola(Jogo *g) {
    int ny, nx;

    apaga_bola(g);

    ny = g->by + g->dy;
    if (ny < F_TOP || ny > F_BOT) {
        g->dy = -g->dy;
        ny = g->by + g->dy;
    }
    nx = g->bx + g->dx;

    if (g->dx < 0 && nx == LP_COL && ny >= g->lp && ny < g->lp + PADDLE_H) {
        int rel = ny - g->lp;
        g->dx = 1;
        nx = LP_COL + 1;
        if (rel == 0) g->dy = -1;
        else if (rel == PADDLE_H - 1) g->dy = 1;
    }
    else if (g->dx > 0 && nx == RP_COL && ny >= g->rp && ny < g->rp + PADDLE_H) {
        int rel = ny - g->rp;
        g->dx = -1;
        nx = RP_COL - 1;
        if (rel == 0) g->dy = -1;
        else if (rel == PADDLE_H - 1) g->dy = 1;
    }

    if (nx < F_LEFT)  { g->sr++; saque(g, 1);  desenha_placar(g); desenha_bola(g); return 1; }
    if (nx > F_RIGHT) { g->sl++; saque(g, -1); desenha_placar(g); desenha_bola(g); return 1; }

    g->bx = nx;
    g->by = ny;
    desenha_bola(g);
    return 0;
}

/* >>>>>>>>>> IMPLEMENTACAO 4: Computador (adversario no modo 1 jogador) (INICIO)
 * Inserir antes de partida() */
/* A cada 3 quadros a raquete direita se aproxima da altura da bola, e so
 * reage quando a bola vem em sua direcao. Como a bola e mais rapida que a
 * raquete, o computador pode errar (da para ganhar). */
static void ia_move(Jogo *g, int frame) {
    if (frame % 3 != 0 || g->dx <= 0) return;

    int centro = g->rp + PADDLE_H / 2;
    if (g->by < centro)
        xymovebar(RP_COL, &g->rp, PADDLE_H, -1, F_TOP, F_BOT, "█", cor_raquete);
    else if (g->by > centro)
        xymovebar(RP_COL, &g->rp, PADDLE_H, +1, F_TOP, F_BOT, "█", cor_raquete);
}
/* <<<<<<<<<< FIM DA IMPLEMENTACAO 4 */

/* >>>>>>>>>> IMPLEMENTACAO 5: Leitura de teclas por modo (INICIO)
 * Substituir le_teclas() da Parte 2 */
/* Retorna: 0 = sair para o menu, 1 = continuar, 2 = mostrar pontuacao.
 * No modo 1 jogador, W/S e as setas movem a raquete esquerda; no modo
 * 2 jogadores, as setas movem a raquete direita. */
static int le_teclas(Jogo *g) {
    int k;
    while ((k = xygetkey()) != ERR) {
        int sobe = (k == 'w' || k == 'W');
        int desce = (k == 's' || k == 'S');
        if (modo == 1) {
            sobe  = sobe  || k == KEY_UP;
            desce = desce || k == KEY_DOWN;
        }

        if (sobe)
            xymovebar(LP_COL, &g->lp, PADDLE_H, -1, F_TOP, F_BOT, "█", cor_raquete);
        else if (desce)
            xymovebar(LP_COL, &g->lp, PADDLE_H, +1, F_TOP, F_BOT, "█", cor_raquete);
        else if (modo == 2 && k == KEY_UP)
            xymovebar(RP_COL, &g->rp, PADDLE_H, -1, F_TOP, F_BOT, "█", cor_raquete);
        else if (modo == 2 && k == KEY_DOWN)
            xymovebar(RP_COL, &g->rp, PADDLE_H, +1, F_TOP, F_BOT, "█", cor_raquete);
        else if (k == 'q' || k == 'Q')
            return 0;
        else if ((k == 'p' || k == 'P') && modo == 1)
            return 2;
    }
    return 1;
}
/* <<<<<<<<<< FIM DA IMPLEMENTACAO 5 */

/* >>>>>>>>>> IMPLEMENTACAO 6: Caixa de pontuacao (modo 1 jogador) (INICIO)
 * Inserir antes de partida() */
/* Abre uma caixa por cima da tela. Se g != NULL mostra tambem o placar da
 * partida em andamento (usada pela tecla P). Com g == NULL e a opcao do
 * menu principal. Espera qualquer tecla para fechar. */
static void caixa_pontuacao(const Jogo *g) {
    char buf[64];

    xyclearrect(8, 22, 17, 57);
    xywindow(8, 22, 17, 57, 2);
    xyprint(9, 24, "PONTUAÇÃO - 1 JOGADOR");

    if (g) {
        snprintf(buf, sizeof buf, "Agora:   Você %d x %d Computador", g->sl, g->sr);
        xyprint(11, 24, buf);
    }
    snprintf(buf, sizeof buf, "Partidas: %d", est.partidas);
    xyprint(12, 24, buf);
    snprintf(buf, sizeof buf, "Vitórias: %d   Derrotas: %d", est.vitorias, est.derrotas);
    xyprint(13, 24, buf);
    snprintf(buf, sizeof buf, "Melhor saldo de pontos: %d", est.melhor_saldo);
    xyprint(14, 24, buf);
    snprintf(buf, sizeof buf, "Última partida: %d x %d", est.ult_sl, est.ult_sr);
    xyprint(15, 24, buf);
    xyprint(16, 24, "Pressione qualquer tecla...");

    xytimeout(-1);
    xyflush();
    xygetkey();
}
/* <<<<<<<<<< FIM DA IMPLEMENTACAO 6 */

/* >>>>>>>>>> IMPLEMENTACAO 7: Partida com os dois modos (INICIO)
 * Substituir partida() da Parte 2 */
/* Retorna 1 para jogar de novo (mesmo modo), 0 para voltar ao menu. */
static int partida(Jogo *g) {
    int frame = 0, r;

    novo_jogo(g);
    xytimeout(0);

    while (g->sl < WIN_SCORE && g->sr < WIN_SCORE) {
        r = le_teclas(g);
        if (r == 0) return 0;
        if (r == 2) {                    /* botao de pontuacao */
            caixa_pontuacao(g);
            redesenha_campo(g);
            xytimeout(0);
            xyflush();
        }
        if (modo == 1) ia_move(g, frame);
        if (++frame % BALL_EVERY == 0) move_bola(g);
        xysleep(FRAME_MS);
    }

    if (modo == 1) {                     /* registra estatisticas */
        est.partidas++;
        if (g->sl > g->sr) est.vitorias++; else est.derrotas++;
        if (g->sl - g->sr > est.melhor_saldo) est.melhor_saldo = g->sl - g->sr;
        est.ult_sl = g->sl;
        est.ult_sr = g->sr;
    }

    if (modo == 1)
        xyprintcenter(10, SCR_W, g->sl > g->sr ? "VOCE VENCEU!" : "O COMPUTADOR VENCEU!");
    else
        xyprintcenter(10, SCR_W, g->sl > g->sr ? "JOGADOR 1 VENCEU!" : "JOGADOR 2 VENCEU!");
    xyprintcenter(12, SCR_W, "R ou ENTER: nova partida   Q: menu");

    xyflush();
    xytimeout(-1);
    for (;;) {
        int k = xygetkey();
        if (k == 'r' || k == 'R' || k == '\n' || k == KEY_ENTER) return 1;
        if (k == 'q' || k == 'Q') return 0;
    }
}
/* <<<<<<<<<< FIM DA IMPLEMENTACAO 7 */

/* >>>>>>>>>> IMPLEMENTACAO 8: Menu de cores (vermelha, azul, verde) (INICIO)
 * Inserir antes de main() */
/* Submenu que devolve a cor escolhida ou -1 se cancelou. */
static int escolhe_cor(const char *titulo) {
    const char *cores[] = {"Vermelha", "Azul", "Verde"};

    clear_screen();
    xyprint(8, 30, titulo);
    int r = xymenu(10, 33, cores, 3);
    if (r < 0) return -1;
    return XY_VERMELHO + r;             /* 0->vermelho, 1->azul, 2->verde */
}

static void menu_cores(void) {
    const char *itens[] = {"Cor das raquetes", "Cor da bola", "Voltar"};
    int r, c;

    for (;;) {
        clear_screen();
        xyprint(5, 37, "CORES");
        xyprint(7, 27, "Prévia:");                    /* mostra as cores atuais */
        xyprintcolor(7, 35, cor_raquete, "████");
        xyprintcolor(7, 41, cor_bola, "O");

        r = xymenu(10, 31, itens, 3);
        if (r == 0) {
            c = escolhe_cor("Cor das raquetes:");
            if (c > 0) cor_raquete = c;
        } else if (r == 1) {
            c = escolhe_cor("Cor da bola:");
            if (c > 0) cor_bola = c;
        } else {
            return;
        }
    }
}
/* <<<<<<<<<< FIM DA IMPLEMENTACAO 8 */

/* >>>>>>>>>> IMPLEMENTACAO 9: Menu principal e main (INICIO)
 * Substituir main() da Parte 2 */
/* Retorna a opcao escolhida (0..4) ou -1 se o usuario apertou Q. */
static int menu_principal(void) {
    const char *itens[] = {"1 Jogador", "2 Jogadores", "Pontuação (1 Jogador)",
                           "Cores", "Sair"};
    clear_screen();
    xywindow(4, 20, 20, 59, 2);
    xyprintcenter(6, SCR_W, "P O N G");
    xyprintcenter(8, SCR_W, "Escolha uma opcao:");
    xyprintcenter(18, SCR_W, "Setas: navegar   ENTER: escolher");
    return xymenu(10, 28, itens, 5);
}

int main(void) {
    Jogo g;
    int op;

    srand((unsigned)time(NULL));
    init_screen(SCR_H, SCR_W);
    xyecho(0);
    xycursor(0);
    xycolor_init();

    while ((op = menu_principal()) != 4 && op != -1) {
        switch (op) {
            case 0: modo = 1; while (partida(&g)) ; break;
            case 1: modo = 2; while (partida(&g)) ; break;
            case 2: caixa_pontuacao(NULL);          break;
            case 3: menu_cores();                   break;
        }
    }

    xycursor(1);
    end_screen();
    return 0;
}
/* <<<<<<<<<< FIM DA IMPLEMENTACAO 9 */
