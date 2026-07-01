#ifndef LISTA_H
#define LISTA_H

typedef struct lista Lista;

typedef int (*PredicadoLista)(const void *dado, void *ctx);
typedef void (*LiberaDadoLista)(void *dado);

Lista *cria_lista(void);
void libera_lista(Lista *lista, LiberaDadoLista libera_dado);
int lista_quantidade(const Lista *lista);
int lista_insere_fim(Lista *lista, void *dado);
void *lista_no_indice(const Lista *lista, int indice);
void *lista_busca(const Lista *lista, PredicadoLista pred, void *ctx);
int lista_remove(Lista *lista, PredicadoLista pred, void *ctx,
                 LiberaDadoLista libera_dado);

#endif
