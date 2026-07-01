#include "time.h"

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

void init_time(Time *t, int id, const char *nome) {
    t->id = id;
    strncpy(t->nome, nome, TIME_NOME_MAX - 1);
    t->nome[TIME_NOME_MAX - 1] = '\0';
    zera_stats(t);
}

void zera_stats(Time *t) {
    t->v = 0;
    t->e = 0;
    t->d = 0;
    t->gm = 0;
    t->gs = 0;
}

int saldo_gols(const Time *t) {
    return t->gm - t->gs;
}

int calc_pontos(const Time *t) {
    return 3 * t->v + t->e;
}

void imprime_cabecalho_times(void) {
    printf("ID  ");
    imprime_campo_alinhado("Time", LARGURA_NOME_TIME);
    printf(" %3s %3s %3s %3s %3s %4s %4s\n",
           "V", "E", "D", "GM", "GS", "S", "PG");
    printf("--------------------------------------------------\n");
}

void imprime_linha_time(const Time *t) {
    if (t == NULL) {
        return;
    }

    printf("%-3d ", t->id);
    imprime_campo_alinhado(t->nome, LARGURA_NOME_TIME);
    printf(" %3d %3d %3d %3d %3d %4d %4d\n",
           t->v, t->e, t->d, t->gm, t->gs,
           saldo_gols(t), calc_pontos(t));
}
