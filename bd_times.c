#include "bd_times.h"

#include "bd_partidas.h"
#include "lista.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct bd_times {
    Lista *times;
};

typedef struct {
    int id;
} CtxIdTime;

static void libera_time(void *dado) {
    free(dado);
}

static int pred_id_time(const void *dado, void *ctx) {
    const Time *t = dado;
    const CtxIdTime *c = ctx;

    return t->id == c->id;
}

static int comeca_com(const char *nome, const char *prefixo) {
    size_t i;
    size_t n = strlen(prefixo);

    if (n == 0) {
        return 0;
    }

    for (i = 0; i < n; i++) {
        if (tolower((unsigned char) nome[i]) != tolower((unsigned char) prefixo[i])) {
            return 0;
        }
    }

    return 1;
}

static int compara_times_classificacao(const void *a, const void *b) {
    const Time * const *pa = a;
    const Time * const *pb = b;
    const Time *t1 = *pa;
    const Time *t2 = *pb;
    int pg1 = calc_pontos(t1);
    int pg2 = calc_pontos(t2);
    int s1 = saldo_gols(t1);
    int s2 = saldo_gols(t2);

    if (pg1 != pg2) {
        return pg2 - pg1;
    }

    if (t1->v != t2->v) {
        return t2->v - t1->v;
    }

    if (s1 != s2) {
        return s2 - s1;
    }

    return t1->id - t2->id;
}

static int monta_vetor_times(const BDTimes *bd, Time **vetor, int capacidade) {
    int i;
    int total = quantidade_times(bd);

    if (total > capacidade) {
        return 0;
    }

    for (i = 0; i < total; i++) {
        vetor[i] = time_no_indice((BDTimes *) bd, i);
        if (vetor[i] == NULL) {
            return 0;
        }
    }

    return total;
}

BDTimes *cria_times_bd(void) {
    BDTimes *bd = malloc(sizeof(BDTimes));

    if (bd == NULL) {
        return NULL;
    }

    bd->times = cria_lista();
    if (bd->times == NULL) {
        free(bd);
        return NULL;
    }

    return bd;
}

void libera_times_bd(BDTimes *bd) {
    if (bd == NULL) {
        return;
    }

    libera_lista(bd->times, libera_time);
    free(bd);
}

int quantidade_times(const BDTimes *bd) {
    if (bd == NULL) {
        return 0;
    }

    return lista_quantidade(bd->times);
}

Time *time_no_indice(BDTimes *bd, int indice) {
    if (bd == NULL) {
        return NULL;
    }

    return (Time *) lista_no_indice(bd->times, indice);
}

int carrega_times(BDTimes *bd, const char *arquivo) {
    FILE *fp = fopen(arquivo, "r");
    char linha[256];
    int id;
    char nome[TIME_NOME_MAX];
    int carregados = 0;

    if (bd == NULL || arquivo == NULL || fp == NULL) {
        return 0;
    }

    if (fgets(linha, sizeof(linha), fp) == NULL) {
        fclose(fp);
        return 0;
    }

    while (fgets(linha, sizeof(linha), fp) != NULL) {
        Time *t;

        if (sscanf(linha, " %d , %49[^,\r\n]", &id, nome) != 2) {
            continue;
        }

        if (carregados >= MAX_TIMES) {
            fclose(fp);
            return 0;
        }

        t = malloc(sizeof(Time));
        if (t == NULL) {
            fclose(fp);
            return 0;
        }

        init_time(t, id, nome);

        if (!lista_insere_fim(bd->times, t)) {
            free(t);
            fclose(fp);
            return 0;
        }

        carregados++;
    }

    fclose(fp);
    return carregados > 0;
}

void zera_todos_stats(BDTimes *bd) {
    int i;
    int total = quantidade_times(bd);

    if (bd == NULL) {
        return;
    }

    for (i = 0; i < total; i++) {
        Time *t = time_no_indice(bd, i);

        if (t != NULL) {
            zera_stats(t);
        }
    }
}

static void soma_jogo(Time *t, int gols_feitos, int gols_levados) {
    t->gm += gols_feitos;
    t->gs += gols_levados;

    if (gols_feitos > gols_levados) {
        t->v++;
    } else if (gols_feitos < gols_levados) {
        t->d++;
    } else {
        t->e++;
    }
}

void atualiza_pontuacao(BDTimes *bd, const BDPartidas *partidas) {
    int i;
    int total = quantidade_partidas(partidas);

    if (bd == NULL || partidas == NULL) {
        return;
    }

    zera_todos_stats(bd);

    for (i = 0; i < total; i++) {
        const Partida *p = partida_no_indice(partidas, i);
        Time *t1;
        Time *t2;

        if (p == NULL) {
            continue;
        }

        t1 = acha_time_id(bd, p->id_time1);
        t2 = acha_time_id(bd, p->id_time2);

        if (t1 == NULL || t2 == NULL) {
            continue;
        }

        soma_jogo(t1, p->gols_time1, p->gols_time2);
        soma_jogo(t2, p->gols_time2, p->gols_time1);
    }
}

Time *acha_time_id(BDTimes *bd, int id) {
    CtxIdTime ctx;

    if (bd == NULL) {
        return NULL;
    }

    ctx.id = id;
    return (Time *) lista_busca(bd->times, pred_id_time, &ctx);
}

const char *nome_do_time(const BDTimes *bd, int id) {
    Time *t;

    if (bd == NULL) {
        return "?";
    }

    t = acha_time_id((BDTimes *) bd, id);
    if (t == NULL) {
        return "?";
    }

    return t->nome;
}

int busca_time_prefixo(const BDTimes *bd, const char *prefixo) {
    int i;
    int achou = 0;
    int total = quantidade_times(bd);

    if (bd == NULL || prefixo == NULL) {
        return 0;
    }

    printf("\n");
    imprime_cabecalho_times();

    for (i = 0; i < total; i++) {
        Time *t = time_no_indice((BDTimes *) bd, i);

        if (t != NULL && comeca_com(t->nome, prefixo)) {
            imprime_linha_time(t);
            achou++;
        }
    }

    if (achou == 0) {
        printf("Nenhum time encontrado para o prefixo informado.\n");
    }

    return achou;
}

static void imprime_classificacao_ordenada(const BDTimes *bd) {
    Time *vetor[MAX_TIMES];
    int total;
    int i;

    total = monta_vetor_times(bd, vetor, MAX_TIMES);
    if (total <= 0) {
        return;
    }

    qsort(vetor, (size_t) total, sizeof(Time *), compara_times_classificacao);

    for (i = 0; i < total; i++) {
        imprime_linha_time(vetor[i]);
    }
}

void mostra_classificacao(const BDTimes *bd) {
    if (bd == NULL) {
        return;
    }

    printf("Imprimindo classificação...\n");
    printf("\n");
    imprime_cabecalho_times();
    imprime_classificacao_ordenada(bd);
}

int grava_classificacao_csv(const BDTimes *bd, const char *arquivo) {
    FILE *fp;
    Time *vetor[MAX_TIMES];
    int total;
    int i;

    if (bd == NULL || arquivo == NULL) {
        return 0;
    }

    fp = fopen(arquivo, "w");
    if (fp == NULL) {
        return 0;
    }

    fprintf(fp, "ID,Time,V,E,D,GM,GS,S,PG\n");

    total = monta_vetor_times(bd, vetor, MAX_TIMES);
    if (total <= 0) {
        fclose(fp);
        return 0;
    }

    qsort(vetor, (size_t) total, sizeof(Time *), compara_times_classificacao);

    for (i = 0; i < total; i++) {
        const Time *t = vetor[i];

        fprintf(fp, "%d,%s,%d,%d,%d,%d,%d,%d,%d\n",
                t->id, t->nome, t->v, t->e, t->d, t->gm, t->gs,
                saldo_gols(t), calc_pontos(t));
    }

    fclose(fp);
    return 1;
}
