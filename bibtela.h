#ifndef _BIBTELA
#define _BIBTELA

/*
* Includes
*/
#include <stdio.h>
#include <stdlib.h>
#include <ncursesw/ncurses.h>
#include <unistd.h>
#include <locale.h>
#include <string.h>
#include <wchar.h>
#include <time.h>    /* [NOVO] time() para srand() no jogo */

/*
* Function prototypes
*/
void init_screen(int x, int y);
void end_screen(void);
void clear_screen(void);
void xygoto(int x, int y);
int xyprint(int x, int y, const char* str);
void xyprintch(int x, int y, const char ch);
void xyprintint(int x, int y, const char* mask, const int num);
void xyprintfloat(int x, int y, const char* mask, const float num);

void xyhline(int x, int y1, int y2, const char *ch);
void xyvline(int y, int x1, int x2, const char *ch);
void xywindow(int x1, int y1, int x2, int y2, int border);
void xyscanfstr(int x, int y, const char* mask, char* str);
void xyscanfint(int x, int y, const char* mask, int* number);
void xyscanfloat(int x, int y, const char* mask, float* number);
char xygetch(int x, int y);

/* >>>>>>>>>> [NOVO] INICIO das declaracoes da equipe */
/* [NOVO] Cores aceitas por xyprintcolor() e xymovebar() */
#define XY_PADRAO    0   /* cor padrao do terminal */
#define XY_VERMELHO  1
#define XY_AZUL      2
#define XY_VERDE     3

/* [NOVO] Interacao e movimentacao */
void xyecho(int enable);
void xycursor(int visible);
void xytimeout(int ms);
int  xygetkey(void);
void xyflush(void);
void xysleep(int ms);
void xyrefresh(void);
void xyprintcenter(int x, int width, const char* str);
void xyclearrect(int x1, int y1, int x2, int y2);
int  xymovebar(int col, int *top, int height, int dir,
               int min_line, int max_line, const char *ch, int cor);

/* [NOVO] Cores e menu */
void xycolor_init(void);
int  xyprintcolor(int x, int y, int cor, const char* str);
int  xymenu(int x, int y, const char *itens[], int n);
/* <<<<<<<<<< [NOVO] FIM das declaracoes da equipe */

#endif
