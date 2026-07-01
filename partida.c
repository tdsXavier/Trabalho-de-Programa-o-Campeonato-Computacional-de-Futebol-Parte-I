#include "partida.h"

#include <stdio.h>
#include <string.h>

#define LARGURA_NOME_TIME 15

static int largura_texto(const char *texto) {
    int i = 0;
    int colunas = 0;

    if (texto == NULL) {
        return 0;
    }

    while (texto[i] != '\0') {
        unsigned char c = (unsigned char) texto[i];

        if (c < 0x80) {
            i++;
        } else if (c < 0xE0) {
            i += 2;
        } else if (c < 0xF0) {
            i += 3;
        } else {
            i += 4;
        }

        colunas++;
    }

    return colunas;
}

static void imprime_campo_alinhado(const char *texto, int largura) {
    int colunas = largura_texto(texto);
    int i;

    fputs(texto, stdout);

    for (i = colunas; i < largura; i++) {
        putchar(' ');
    }
}

void init_partida(Partida *p, int id, int t1, int t2, int g1, int g2) {
    p->id = id;
    p->id_time1 = t1;
    p->id_time2 = t2;
    p->gols_time1 = g1;
    p->gols_time2 = g2;
}

void imprime_cabecalho_partidas(void) {
    printf("ID  ");
    imprime_campo_alinhado("Time1", LARGURA_NOME_TIME);
    printf(" ");
    imprime_campo_alinhado("Time2", LARGURA_NOME_TIME);
    printf(" %7s %7s\n", "Placar1", "Placar2");
    printf("-------------------------------------------------------\n");
}

void imprime_linha_partida(int id, const char *time1, const char *time2,
                           int placar1, int placar2) {
    printf("%-3d ", id);
    imprime_campo_alinhado(time1, LARGURA_NOME_TIME);
    printf(" ");
    imprime_campo_alinhado(time2, LARGURA_NOME_TIME);
    printf(" %7d %7d\n", placar1, placar2);
}
