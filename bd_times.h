#ifndef BD_TIMES_H
#define BD_TIMES_H

#include "time.h"

/* TAD BDTimes: definicao da struct em bd_times.c */
typedef struct bd_times BDTimes;
typedef struct bd_partidas BDPartidas;

#define MAX_TIMES 16

BDTimes *cria_times_bd(void);
void libera_times_bd(BDTimes *bd);
int carrega_times(BDTimes *bd, const char *arquivo);
void zera_todos_stats(BDTimes *bd);
void atualiza_pontuacao(BDTimes *bd, const BDPartidas *partidas);
Time *acha_time_id(BDTimes *bd, int id);
const char *nome_do_time(const BDTimes *bd, int id);
int quantidade_times(const BDTimes *bd);
Time *time_no_indice(BDTimes *bd, int indice);
int busca_time_prefixo(const BDTimes *bd, const char *prefixo);
void mostra_classificacao(const BDTimes *bd);
int grava_classificacao_csv(const BDTimes *bd, const char *arquivo);

#endif
