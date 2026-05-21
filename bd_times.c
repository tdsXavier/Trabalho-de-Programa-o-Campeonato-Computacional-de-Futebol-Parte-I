#include "bd_times.h"

#include "bd_partidas.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Busca por prefixo sem diferenciar maiusculas de minusculas */
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

BDTimes *cria_times_bd(void) {
    BDTimes *bd = malloc(sizeof(BDTimes));

    if (bd == NULL) {
        return NULL;
    }

    bd->capacidade = MAX_TIMES;
    bd->qtd_times = 0;
    bd->times = malloc((size_t)bd->capacidade * sizeof(Time));

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

    free(bd->times);
    free(bd);
}

/* Le times.csv e monta o vetor de times */
int carrega_times(BDTimes *bd, const char *arquivo) {
    FILE *fp = fopen(arquivo, "r");
    char linha[256];
    int id;
    char nome[TIME_NOME_MAX];

    if (bd == NULL || arquivo == NULL || fp == NULL) {
        return 0;
    }

    bd->qtd_times = 0;

    if (fgets(linha, sizeof(linha), fp) == NULL) {
        fclose(fp);
        return 0;
    }

    while (fgets(linha, sizeof(linha), fp) != NULL) {
        if (bd->qtd_times >= bd->capacidade) {
            fclose(fp);
            return 0;
        }

        if (sscanf(linha, " %d , %49[^,\r\n]", &id, nome) != 2) {
            continue;
        }

        init_time(&bd->times[bd->qtd_times], id, nome);
        bd->qtd_times++;
    }

    fclose(fp);
    return bd->qtd_times > 0;
}

void zera_todos_stats(BDTimes *bd) {
    int i;

    if (bd == NULL) {
        return;
    }

    for (i = 0; i < bd->qtd_times; i++) {
        zera_stats(&bd->times[i]);
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

/*
 * Percorre as partidas e recalcula V, E, D, GM e GS de cada time.
 * Time1 da partida e o mandante (gols_time1).
 */
void atualiza_pontuacao(BDTimes *bd, const BDPartidas *partidas) {
    int i;

    if (bd == NULL || partidas == NULL) {
        return;
    }

    zera_todos_stats(bd);

    for (i = 0; i < partidas->qtd_partidas; i++) {
        const Partida *p = &partidas->partidas[i];
        Time *t1 = acha_time_id(bd, p->id_time1);
        Time *t2 = acha_time_id(bd, p->id_time2);

        if (t1 == NULL || t2 == NULL) {
            continue;
        }

        soma_jogo(t1, p->gols_time1, p->gols_time2);
        soma_jogo(t2, p->gols_time2, p->gols_time1);
    }
}

Time *acha_time_id(BDTimes *bd, int id) {
    int i;

    if (bd == NULL) {
        return NULL;
    }

    for (i = 0; i < bd->qtd_times; i++) {
        if (bd->times[i].id == id) {
            return &bd->times[i];
        }
    }

    return NULL;
}

const char *nome_do_time(const BDTimes *bd, int id) {
    int i;

    if (bd == NULL) {
        return "?";
    }

    for (i = 0; i < bd->qtd_times; i++) {
        if (bd->times[i].id == id) {
            return bd->times[i].nome;
        }
    }

    return "?";
}

int busca_time_prefixo(const BDTimes *bd, const char *prefixo) {
    int i;
    int achou = 0;

    if (bd == NULL || prefixo == NULL) {
        return 0;
    }

    printf("ID Time V E D GM GS S PG\n");

    for (i = 0; i < bd->qtd_times; i++) {
        if (comeca_com(bd->times[i].nome, prefixo)) {
            imprime_linha_time(&bd->times[i]);
            achou++;
        }
    }

    if (achou == 0) {
        printf("Nenhum time encontrado para o prefixo informado.\n");
    }

    return achou;
}

void mostra_classificacao(const BDTimes *bd) {
    int i;

    if (bd == NULL) {
        return;
    }

    printf("Imprimindo classificação...\n");
    printf("ID Time V E D GM GS S PG\n");

    for (i = 0; i < bd->qtd_times; i++) {
        imprime_linha_time(&bd->times[i]);
    }
}

/* Grava bd_classificacao.csv para entrega (chamado na opcao 6) */
int grava_classificacao_csv(const BDTimes *bd, const char *arquivo) {
    FILE *fp;
    int i;

    if (bd == NULL || arquivo == NULL) {
        return 0;
    }

    fp = fopen(arquivo, "w");
    if (fp == NULL) {
        return 0;
    }

    fprintf(fp, "ID,Time,V,E,D,GM,GS,S,PG\n");

    for (i = 0; i < bd->qtd_times; i++) {
        const Time *t = &bd->times[i];
        fprintf(fp, "%d,%s,%d,%d,%d,%d,%d,%d,%d\n",
                t->id, t->nome, t->v, t->e, t->d, t->gm, t->gs,
                saldo_gols(t), calc_pontos(t));
    }

    fclose(fp);
    return 1;
}
