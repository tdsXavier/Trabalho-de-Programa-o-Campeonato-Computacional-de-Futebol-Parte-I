#ifndef BD_PARTIDAS_H
#define BD_PARTIDAS_H

#include "partida.h"

struct BDTimes;

#define MAX_PARTIDAS 128

typedef struct BDPartidas {
    Partida *partidas;
    int qtd_partidas;
    int capacidade;
} BDPartidas;

BDPartidas *cria_partidas_bd(void);
void libera_partidas_bd(BDPartidas *bd);
int carrega_partidas(BDPartidas *bd, const char *arquivo);
int busca_partidas(const BDPartidas *bd, const struct BDTimes *times,
                   int modo, const char *prefixo);

#endif
