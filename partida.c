#include "partida.h"

void init_partida(Partida *p, int id, int t1, int t2, int g1, int g2) {
    p->id = id;
    p->id_time1 = t1;
    p->id_time2 = t2;
    p->gols_time1 = g1;
    p->gols_time2 = g2;
}
