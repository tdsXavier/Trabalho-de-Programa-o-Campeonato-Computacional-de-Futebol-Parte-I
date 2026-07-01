#ifndef TIME_H
#define TIME_H

#define TIME_NOME_MAX 50

typedef struct {
    int id;
    char nome[TIME_NOME_MAX];
    int v;
    int e;
    int d;
    int gm;
    int gs;
} Time;

void init_time(Time *t, int id, const char *nome);
void zera_stats(Time *t);
int saldo_gols(const Time *t);
int calc_pontos(const Time *t);
void imprime_cabecalho_times(void);
void imprime_linha_time(const Time *t);

#endif
