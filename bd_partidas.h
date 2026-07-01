#ifndef BD_PARTIDAS_H
#define BD_PARTIDAS_H

#include "partida.h"

typedef struct bd_partidas BDPartidas;
typedef struct bd_times BDTimes;

BDPartidas *cria_partidas_bd(void);
void libera_partidas_bd(BDPartidas *bd);
int carrega_partidas(BDPartidas *bd, const char *arquivo);
int quantidade_partidas(const BDPartidas *bd);
const Partida *partida_no_indice(const BDPartidas *bd, int indice);
Partida *busca_partida_id(BDPartidas *bd, int id);
int busca_partidas(const BDPartidas *bd, const BDTimes *times,
                   int modo, const char *prefixo);
int atualiza_placar_partida(BDPartidas *bd, int id, int g1, int g2);
int remove_partida_id(BDPartidas *bd, int id);
int insere_partida(BDPartidas *bd, int t1, int t2, int g1, int g2);
int grava_partidas_csv(const BDPartidas *bd, const char *arquivo);
int proximo_id_partida(const BDPartidas *bd);

#endif
