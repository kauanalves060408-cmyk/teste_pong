/*
* Function implementations
*
* MUDANCAS DA EQUIPE (todas marcadas por comentarios neste arquivo):
*   [ALTERADO] init_screen  - setlocale + keypad
*   [ALTERADO] clear_screen - wrefresh
*   [ALTERADO] xywindow     - comentario corrigido
*   [NOVO]     cores_ativas (variavel global)
*   [NOVO]     bloco "INTERACAO E MOVIMENTACAO": xyecho, xycursor, xytimeout,
*              xygetkey, xyflush, xysleep, xyrefresh, xyprintcenter,
*              xyclearrect, xymovebar
*   [NOVO]     bloco "CORES": xycolor_init, xyprintcolor
*   [NOVO]     bloco "MENU": xymenu
*/
#include "bibtela.h"

/*
* Global variables
*/
WINDOW *my_screen;   // main global screen

/* [NOVO] Indica se as cores foram habilitadas por xycolor_init().
 * Se o terminal nao suportar cores, as funcoes coloridas imprimem
 * normalmente, sem cor. */
static int cores_ativas = 0;

/**
 * Create and define main screen dimensions
 * 
 * @param[in] x qty. of lines.
 * @param[in] y qty. of columns.
 * 
 */
void init_screen(int x, int y) {
    /* [ALTERADO] setlocale: sem isso o ncurses nao exibe corretamente os
     * caracteres Unicode (bordas de xywindow e o bloco usado nas raquetes). */
    setlocale(LC_ALL, "");
    initscr();
    my_screen = newwin(x, y, 0, 0);
    cbreak();
    echo();
    /* [ALTERADO] keypad: habilita teclas especiais (setas, F1...). Sem
     * isso, as setas chegam como sequencias de escape e nao como KEY_UP. */
    keypad(my_screen, TRUE);
    wrefresh(my_screen);
}


/**
 * End screen created by ini_screen function.
 * 
 */
void end_screen(void) {
    endwin();
}


/**
 * Clear all screen.
 * 
 */
void clear_screen(void) {
    wclear(my_screen);
    /* [ALTERADO] wrefresh: antes, a tela so era limpa de verdade no proximo
     * refresh de outra funcao. Agora a limpeza aparece imediatamente. */
    wrefresh(my_screen);
}


/**
 * Move the cursor to coordinate defined by x line and y column.
 * 
 * @param[in] x coordinate line.
 * @param[in] y coordenate column.
 * 
 */
void xygoto(int x, int y) {
    wmove(my_screen, x, y);
    wrefresh(my_screen);
}


/**
 * Print a string from x line and y column.
 * 
 * @param[in] x line of init point to show the string.
 * @param[in] y column of init point to show the string.
 * @param[in] str string to print.
 * @return Returns lenght of string printed.
 * 
 */
int xyprint(int x, int y, const char* str) {
    mvwprintw(my_screen, x, y, "%s", str);
    wrefresh(my_screen);
    return strlen(str);
}

/**
 * Print a char from x line and y column.
 * 
 * @param[in] x line of init point to show the string.
 * @param[in] y column of init point to show the string.
 * @param[in] ch char to print.
 * 
 */
void xyprintch(int x, int y, const char ch) {
    mvwaddch(my_screen, x, y, ch);
    wrefresh(my_screen);
}


/**
 * Print a integer from x line and y column.
 * 
 * @param[in] x line of init point to show the string.
 * @param[in] y column of init point to show the string.
 * @param[in] mask string of mask to print.
 * @param[in] num integer number to print.
 * 
 */
void xyprintint(int x, int y, const char* mask, const int num) {
    mvwprintw(my_screen, x, y, mask, num);
    wrefresh(my_screen);
}


/**
 * Print a float from x line and y column.
 * 
 * @param[in] x line of init point to show the string.
 * @param[in] y column of init point to show the string.
 * @param[in] mask string of mask to print.
 * @param[in] num float number to print.
 * 
 */
void xyprintfloat(int x, int y, const char* mask, const float num) {
    mvwprintw(my_screen, x, y, mask, num);
    wrefresh(my_screen);
}

/**
 * Input a string like scanf() on a screen coordinate.
 * 
 * @param[in] x line of init point to show the string.
 * @param[in] y column of init point to show the string.
 * @param[in] mask string of mask to print.
 * @param[in] str input string.
 * 
 */
void xyscanfstr(int x, int y, const char* mask, char* str) {
    xygoto(x, y);
    wscanw(my_screen, mask, str);
}


/**
 * Input a integer number like scanf() on a screen coordinate.
 * 
 * @param[in] x line of init point to show the string.
 * @param[in] y column of init point to show the string.
 * @param[in] mask string of mask to print.
 * 
 */
void xyscanfint(int x, int y, const char* mask, int* number) {
    xygoto(x, y);
    wscanw(my_screen, mask, number);
}


/**
 * Input a integer number like scanf() on a screen coordinate.
 * 
 * @param[in] x line of init point to show the string.
 * @param[in] y column of init point to show the string.
 * @param[in] mask string of mask to print.
 * 
 */
void xyscanfloat(int x, int y, const char* mask, float* number) {
    xygoto(x, y);
    wscanw(my_screen, mask, number);
}

/**
 * Input a char like getch() on a screen coordinate.
 * 
 * @param[in] x line of init point to show the string.
 * @param[in] y column of init point to show the string.
 * @param[out] input char.
 * 
 */
char xygetch(int x, int y) {
    char ch[1];
    mvwscanw(my_screen, x, y, "%c", ch);
    return ch[0];
}


/**
 * Show a horizontal line.
 * 
 * @param[in] x line of line.
 * @param[in] y1 init column.
 * @param[in] y2 end column.
 * @param[in] ch character to draw the line (string type).
 * 
 */
void xyhline(int x, int y1, int y2, const char *ch) {
    for (int y=y1; y<=y2; y++) 
        xyprint(x, y, ch);
}

/**
 * Show a vertical line.
 * 
 * @param[in] y column of line.
 * @param[in] x1 init line.
 * @param[in] x2 end line.
 * @param[in] ch character to draw the line (string type).
 * 
 */
void xyvline(int y, int x1, int x2, const char *ch) {   
    for (int x=x1; x<=x2; x++) 
        xyprint(x, y, ch);
}

/**
 * [ALTERADO] Comentario corrigido (era copia do end_screen).
 * Draw a window (rectangle with border) on the screen.
 * 
 * @param[in] x1 coordinate line of uppper left coordinate.
 * @param[in] y1 coordenate column of uppper left coordinate.
 * @param[in] x2 coordinate line of down right coordinate.
 * @param[in] y2 coordenate column of down right coordinate.
 * @param[in] border type of window. 1-Single, 2-Double, 3-Bold.
 * 
 */
void xywindow(int x1, int y1, int x2, int y2, int border) {
    char *b[18] = {"┌","┐","┘","└","─","│","╔","╗","╝","╚","═","║","█","█","█","█","█","█"};
    int z=0;

    // desloc index to correct type character set
    z = (border - 1) * 6;

    // uppper left corner
    xygoto(x1, y1);
    wprintw(my_screen, "%s", b[z]);

    // uppper right corner
    xygoto(x1, y2);
    wprintw(my_screen, "%s", b[z+1]);

    // down left corner
    xygoto(x2, y1);
    wprintw(my_screen, "%s", b[z+3]);

    // down right corner
    xygoto(x2, y2);
    wprintw(my_screen, "%s", b[z+2]);

    // Horizontal bars
    xyhline(x1, y1+1, y2-1, b[z+4]);
    xyhline(x2, y1+1, y2-1, b[z+4]);

    // Vertical bars
    xyvline(y1, x1+1, x2-1, b[z+5]);
    xyvline(y2, x1+1, x2-1, b[z+5]);

    wrefresh(my_screen);
}


/* >>>>>>>>>> [NOVO] INICIO: INTERACAO E MOVIMENTACAO (versao da equipe) */

/**
 * [NOVO] Liga ou desliga o eco das teclas digitadas.
 * No jogo o eco precisa ficar desligado, senao as teclas de movimento
 * (W, S) apareceriam escritas na tela.
 *
 * @param[in] enable 1 = eco ligado, 0 = eco desligado.
 */
void xyecho(int enable) {
    if (enable) echo(); else noecho();
}


/**
 * [NOVO] Mostra ou esconde o cursor do terminal.
 * Esconder o cursor evita que ele fique piscando sobre o campo de jogo.
 *
 * @param[in] visible 1 = visivel, 0 = escondido.
 */
void xycursor(int visible) {
    curs_set(visible ? 1 : 0);
}


/**
 * [NOVO] Define quanto tempo xygetkey() espera por uma tecla.
 * E o que permite o jogo continuar rodando sem travar esperando o usuario.
 *
 * @param[in] ms  <0 = bloqueia ate uma tecla ser pressionada;
 *                 0 = nao espera (devolve ERR se nao houver tecla);
 *                >0 = espera no maximo ms milissegundos.
 */
void xytimeout(int ms) {
    wtimeout(my_screen, ms);
}


/**
 * [NOVO] Le uma tecla, inclusive teclas especiais (setas).
 * Compare o retorno com KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, '\n', 'w'...
 * Diferente de xygetch(), nao usa scanw: retorna int e entende as setas.
 *
 * @return codigo da tecla, ou ERR (-1) se nenhuma foi pressionada no tempo
 *         definido por xytimeout().
 */
int xygetkey(void) {
    return wgetch(my_screen);
}


/**
 * [NOVO] Descarta as teclas pendentes no buffer de entrada.
 * Util apos pausas/menus para que teclas antigas nao sejam lidas depois.
 */
void xyflush(void) {
    flushinp();
}


/**
 * [NOVO] Pausa o programa (controla a velocidade do jogo).
 *
 * @param[in] ms milissegundos de espera.
 */
void xysleep(int ms) {
    napms(ms);
}


/**
 * [NOVO] Atualiza a tela principal.
 */
void xyrefresh(void) {
    wrefresh(my_screen);
}


/**
 * [NOVO] Imprime uma string centralizada em uma linha.
 * Obs.: usa strlen (bytes); para textos com acentos o centro pode
 * ficar 1 ou 2 colunas deslocado.
 *
 * @param[in] x linha.
 * @param[in] width largura da area onde o texto sera centralizado.
 * @param[in] str texto.
 */
void xyprintcenter(int x, int width, const char* str) {
    int col = (width - (int)strlen(str)) / 2;
    if (col < 0) col = 0;
    xyprint(x, col, str);
}


/**
 * [NOVO] Limpa uma area retangular (preenche com espacos).
 * Usada para abrir caixas de dialogo por cima do jogo.
 *
 * @param[in] x1,y1 canto superior esquerdo (linha, coluna).
 * @param[in] x2,y2 canto inferior direito (linha, coluna).
 */
void xyclearrect(int x1, int y1, int x2, int y2) {
    for (int x = x1; x <= x2; x++)
        mvwhline(my_screen, x, y1, ' ', y2 - y1 + 1);
    wrefresh(my_screen);
}


/**
 * [NOVO] Move uma barra vertical (raquete) um passo para cima ou para baixo,
 * sem sair dos limites. So apaga e desenha a linha que muda (evita piscar).
 *
 * @param[in]     col      coluna da barra.
 * @param[in,out] top      linha do topo da barra (atualizada se mover).
 * @param[in]     height   altura da barra.
 * @param[in]     dir      -1 = sobe, +1 = desce.
 * @param[in]     min_line primeira linha permitida.
 * @param[in]     max_line ultima linha permitida.
 * @param[in]     ch       string usada para desenhar a barra.
 * @param[in]     cor      cor da barra (XY_PADRAO, XY_VERMELHO, ...).
 * @return 1 se moveu, 0 se foi bloqueada pelo limite.
 */
int xymovebar(int col, int *top, int height, int dir,
              int min_line, int max_line, const char *ch, int cor) {
    int novo = *top + dir;

    if (novo < min_line || novo + height - 1 > max_line)
        return 0;

    if (dir < 0) {                       // sobe: apaga a linha de baixo
        xyprint(*top + height - 1, col, " ");
        xyprintcolor(novo, col, cor, ch);
    } else {                             // desce: apaga a linha de cima
        xyprint(*top, col, " ");
        xyprintcolor(novo + height - 1, col, cor, ch);
    }
    *top = novo;
    return 1;
}

/* <<<<<<<<<< [NOVO] FIM: INTERACAO E MOVIMENTACAO */


/* >>>>>>>>>> [NOVO] INICIO: CORES */

/**
 * [NOVO] Inicializa o sistema de cores do ncurses.
 * Cria os pares de cores usados por XY_VERMELHO, XY_AZUL e XY_VERDE.
 * Chamar uma vez, logo apos init_screen(). Se o terminal nao suportar
 * cores, nada acontece e o jogo continua em preto e branco.
 */
void xycolor_init(void) {
    if (!has_colors()) return;
    start_color();
    // fundo -1 = mantem o fundo padrao do terminal (se suportado)
    int bg = (use_default_colors() == OK) ? -1 : COLOR_BLACK;
    init_pair(XY_VERMELHO, COLOR_RED,   bg);
    init_pair(XY_AZUL,     COLOR_BLUE,  bg);
    init_pair(XY_VERDE,    COLOR_GREEN, bg);
    cores_ativas = 1;
}


/**
 * [NOVO] Imprime uma string colorida a partir da linha x, coluna y.
 * Usa negrito junto com a cor para o azul ficar visivel em fundo escuro.
 * Com cor XY_PADRAO (ou sem suporte a cores) funciona como xyprint().
 *
 * @param[in] x,y posicao (linha, coluna).
 * @param[in] cor XY_PADRAO, XY_VERMELHO, XY_AZUL ou XY_VERDE.
 * @param[in] str texto.
 * @return tamanho da string impressa.
 */
int xyprintcolor(int x, int y, int cor, const char* str) {
    if (!cores_ativas || cor <= 0)
        return xyprint(x, y, str);

    wattron(my_screen, COLOR_PAIR(cor) | A_BOLD);
    mvwprintw(my_screen, x, y, "%s", str);
    wattroff(my_screen, COLOR_PAIR(cor) | A_BOLD);
    wrefresh(my_screen);
    return strlen(str);
}

/* <<<<<<<<<< [NOVO] FIM: CORES */


/* >>>>>>>>>> [NOVO] INICIO: MENU */

/**
 * [NOVO] Menu vertical navegavel com as setas.
 * Seta cima/baixo muda a opcao (destacada em video reverso), ENTER
 * confirma e Q cancela. Bloqueia ate o usuario escolher.
 *
 * @param[in] x,y   posicao do primeiro item (linha, coluna).
 * @param[in] itens vetor de strings com o texto de cada opcao.
 * @param[in] n     quantidade de opcoes.
 * @return indice da opcao escolhida (0..n-1), ou -1 se cancelou com Q.
 */
int xymenu(int x, int y, const char *itens[], int n) {
    int sel = 0, k;

    xytimeout(-1);
    for (;;) {
        for (int i = 0; i < n; i++) {
            if (i == sel) wattron(my_screen, A_REVERSE);
            mvwprintw(my_screen, x + i, y, " %s ", itens[i]);
            if (i == sel) wattroff(my_screen, A_REVERSE);
        }
        wrefresh(my_screen);

        k = wgetch(my_screen);
        if (k == KEY_UP)                       sel = (sel + n - 1) % n;
        else if (k == KEY_DOWN)                sel = (sel + 1) % n;
        else if (k == '\n' || k == KEY_ENTER)  return sel;
        else if (k == 'q' || k == 'Q')         return -1;
    }
}
/* <<<<<<<<<< [NOVO] FIM: MENU */
