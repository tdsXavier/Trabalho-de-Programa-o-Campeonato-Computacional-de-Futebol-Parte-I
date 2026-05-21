#ifndef PARTIDA_H
#define PARTIDA_H

typedef struct {
    int id;
    int id_time1;
    int id_time2;
    int gols_time1;
    int gols_time2;
} Partida;

void init_partida(Partida *p, int id, int t1, int t2, int g1, int g2);

#endif
