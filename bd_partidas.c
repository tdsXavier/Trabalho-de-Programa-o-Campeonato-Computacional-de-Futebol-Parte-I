#include "bd_partidas.h"

#include "bd_times.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

BDPartidas *cria_partidas_bd(void) {
    BDPartidas *bd = malloc(sizeof(BDPartidas));

    if (bd == NULL) {
        return NULL;
    }

    bd->capacidade = MAX_PARTIDAS;
    bd->qtd_partidas = 0;
    bd->partidas = malloc((size_t)bd->capacidade * sizeof(Partida));

    if (bd->partidas == NULL) {
        free(bd);
        return NULL;
    }

    return bd;
}

void libera_partidas_bd(BDPartidas *bd) {
    if (bd == NULL) {
        return;
    }

    free(bd->partidas);
    free(bd);
}

/* Le partidas.csv linha a linha */
int carrega_partidas(BDPartidas *bd, const char *arquivo) {
    FILE *fp = fopen(arquivo, "r");
    char linha[256];
    int id, t1, t2, g1, g2;

    if (bd == NULL || arquivo == NULL || fp == NULL) {
        return 0;
    }

    bd->qtd_partidas = 0;

    if (fgets(linha, sizeof(linha), fp) == NULL) {
        fclose(fp);
        return 1;
    }

    while (fgets(linha, sizeof(linha), fp) != NULL) {
        if (bd->qtd_partidas >= bd->capacidade) {
            fclose(fp);
            return 0;
        }

        if (sscanf(linha, " %d , %d , %d , %d , %d", &id, &t1, &t2, &g1, &g2) != 5) {
            continue;
        }

        init_partida(&bd->partidas[bd->qtd_partidas], id, t1, t2, g1, g2);
        bd->qtd_partidas++;
    }

    fclose(fp);
    return 1;
}

/*
 * modo 1 = mandante, 2 = visitante, 3 = qualquer um dos dois
 * Busca por prefixo do nome do time.
 */
int busca_partidas(const BDPartidas *bd, const BDTimes *times,
                   int modo, const char *prefixo) {
    int i;
    int achou = 0;
    const char *nome1;
    const char *nome2;

    if (bd == NULL || times == NULL || prefixo == NULL) {
        return 0;
    }

    printf("ID Time1 Time2\n");

    for (i = 0; i < bd->qtd_partidas; i++) {
        const Partida *p = &bd->partidas[i];
        int ok = 0;

        nome1 = nome_do_time(times, p->id_time1);
        nome2 = nome_do_time(times, p->id_time2);

        if (modo == 1) {
            ok = comeca_com(nome1, prefixo);
        } else if (modo == 2) {
            ok = comeca_com(nome2, prefixo);
        } else if (modo == 3) {
            ok = comeca_com(nome1, prefixo) || comeca_com(nome2, prefixo);
        }

        if (!ok) {
            continue;
        }

        printf("%d %s %d x %d %s\n",
               p->id, nome1, p->gols_time1, p->gols_time2, nome2);
        achou++;
    }

    if (achou == 0) {
        printf("Nenhuma partida encontrada para o criterio informado.\n");
    }

    return achou;
}
