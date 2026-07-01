#include "bd_partidas.h"

#include "bd_times.h"
#include "lista.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct bd_partidas {
    Lista *partidas;
    int proximo_id;
};

typedef struct {
    int id;
} CtxIdPartida;

static void libera_partida(void *dado) {
    free(dado);
}

static int pred_id_partida(const void *dado, void *ctx) {
    const Partida *p = dado;
    const CtxIdPartida *c = ctx;

    return p->id == c->id;
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

static void atualiza_proximo_id(BDPartidas *bd) {
    int i;
    int total;
    int max_id = -1;

    if (bd == NULL) {
        return;
    }

    total = quantidade_partidas(bd);
    for (i = 0; i < total; i++) {
        const Partida *p = partida_no_indice(bd, i);

        if (p != NULL && p->id > max_id) {
            max_id = p->id;
        }
    }

    bd->proximo_id = max_id + 1;
}

BDPartidas *cria_partidas_bd(void) {
    BDPartidas *bd = malloc(sizeof(BDPartidas));

    if (bd == NULL) {
        return NULL;
    }

    bd->partidas = cria_lista();
    if (bd->partidas == NULL) {
        free(bd);
        return NULL;
    }

    bd->proximo_id = 0;
    return bd;
}

void libera_partidas_bd(BDPartidas *bd) {
    if (bd == NULL) {
        return;
    }

    libera_lista(bd->partidas, libera_partida);
    free(bd);
}

int quantidade_partidas(const BDPartidas *bd) {
    if (bd == NULL) {
        return 0;
    }

    return lista_quantidade(bd->partidas);
}

const Partida *partida_no_indice(const BDPartidas *bd, int indice) {
    if (bd == NULL) {
        return NULL;
    }

    return (const Partida *) lista_no_indice(bd->partidas, indice);
}

Partida *busca_partida_id(BDPartidas *bd, int id) {
    CtxIdPartida ctx;

    if (bd == NULL) {
        return NULL;
    }

    ctx.id = id;
    return (Partida *) lista_busca(bd->partidas, pred_id_partida, &ctx);
}

int proximo_id_partida(const BDPartidas *bd) {
    if (bd == NULL) {
        return 0;
    }

    return bd->proximo_id;
}

int carrega_partidas(BDPartidas *bd, const char *arquivo) {
    FILE *fp = fopen(arquivo, "r");
    char linha[256];
    int id, t1, t2, g1, g2;

    if (bd == NULL || arquivo == NULL || fp == NULL) {
        return 0;
    }

    if (fgets(linha, sizeof(linha), fp) == NULL) {
        fclose(fp);
        return 1;
    }

    while (fgets(linha, sizeof(linha), fp) != NULL) {
        Partida *p;

        if (sscanf(linha, " %d , %d , %d , %d , %d", &id, &t1, &t2, &g1, &g2) != 5) {
            continue;
        }

        p = malloc(sizeof(Partida));
        if (p == NULL) {
            fclose(fp);
            return 0;
        }

        init_partida(p, id, t1, t2, g1, g2);

        if (!lista_insere_fim(bd->partidas, p)) {
            free(p);
            fclose(fp);
            return 0;
        }
    }

    fclose(fp);
    atualiza_proximo_id(bd);
    return 1;
}

int busca_partidas(const BDPartidas *bd, const BDTimes *times,
                   int modo, const char *prefixo) {
    int i;
    int achou = 0;
    int total = quantidade_partidas(bd);

    if (bd == NULL || times == NULL || prefixo == NULL) {
        return 0;
    }

    printf("\n");
    imprime_cabecalho_partidas();

    for (i = 0; i < total; i++) {
        const Partida *p = partida_no_indice(bd, i);
        const char *nome1;
        const char *nome2;
        int ok = 0;

        if (p == NULL) {
            continue;
        }

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

        imprime_linha_partida(p->id, nome1, nome2, p->gols_time1, p->gols_time2);
        achou++;
    }

    if (achou == 0) {
        printf("Nenhuma partida encontrada para o criterio informado.\n");
    }

    return achou;
}

int atualiza_placar_partida(BDPartidas *bd, int id, int g1, int g2) {
    Partida *p;

    if (bd == NULL) {
        return 0;
    }

    p = busca_partida_id(bd, id);
    if (p == NULL) {
        return 0;
    }

    if (g1 >= 0) {
        p->gols_time1 = g1;
    }

    if (g2 >= 0) {
        p->gols_time2 = g2;
    }

    return 1;
}

int remove_partida_id(BDPartidas *bd, int id) {
    CtxIdPartida ctx;

    if (bd == NULL) {
        return 0;
    }

    ctx.id = id;
    return lista_remove(bd->partidas, pred_id_partida, &ctx, libera_partida);
}

int insere_partida(BDPartidas *bd, int t1, int t2, int g1, int g2) {
    Partida *p;

    if (bd == NULL || g1 < 0 || g2 < 0) {
        return 0;
    }

    p = malloc(sizeof(Partida));
    if (p == NULL) {
        return 0;
    }

    init_partida(p, bd->proximo_id, t1, t2, g1, g2);

    if (!lista_insere_fim(bd->partidas, p)) {
        free(p);
        return 0;
    }

    bd->proximo_id++;
    return 1;
}

int grava_partidas_csv(const BDPartidas *bd, const char *arquivo) {
    FILE *fp;
    int i;
    int total;

    if (bd == NULL || arquivo == NULL) {
        return 0;
    }

    fp = fopen(arquivo, "w");
    if (fp == NULL) {
        return 0;
    }

    fprintf(fp, "ID,Time1ID,Time2ID,GolsTime1,GolsTime2\n");

    total = quantidade_partidas(bd);
    for (i = 0; i < total; i++) {
        const Partida *p = partida_no_indice(bd, i);

        if (p == NULL) {
            continue;
        }

        fprintf(fp, "%d,%d,%d,%d,%d\n",
                p->id, p->id_time1, p->id_time2, p->gols_time1, p->gols_time2);
    }

    fclose(fp);
    return 1;
}
