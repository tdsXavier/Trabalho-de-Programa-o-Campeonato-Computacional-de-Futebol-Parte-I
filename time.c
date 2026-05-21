#include "time.h"

#include <stdio.h>
#include <string.h>

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

void imprime_linha_time(const Time *t) {
    printf("%d %s %d %d %d %d %d %d %d\n",
           t->id, t->nome, t->v, t->e, t->d, t->gm, t->gs,
           saldo_gols(t), calc_pontos(t));
}
