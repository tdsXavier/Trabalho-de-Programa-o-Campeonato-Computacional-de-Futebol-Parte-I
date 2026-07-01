#include "lista.h"

#include <stdlib.h>

typedef struct no_lista {
    void *dado;
    struct no_lista *prox;
} NoLista;

struct lista {
    NoLista *inicio;
    int quantidade;
};

Lista *cria_lista(void) {
    Lista *lista = malloc(sizeof(Lista));

    if (lista == NULL) {
        return NULL;
    }

    lista->inicio = NULL;
    lista->quantidade = 0;
    return lista;
}

void libera_lista(Lista *lista, LiberaDadoLista libera_dado) {
    NoLista *atual;

    if (lista == NULL) {
        return;
    }

    atual = lista->inicio;
    while (atual != NULL) {
        NoLista *prox = atual->prox;

        if (libera_dado != NULL) {
            libera_dado(atual->dado);
        }

        free(atual);
        atual = prox;
    }

    free(lista);
}

int lista_quantidade(const Lista *lista) {
    if (lista == NULL) {
        return 0;
    }

    return lista->quantidade;
}

int lista_insere_fim(Lista *lista, void *dado) {
    NoLista *novo;

    if (lista == NULL) {
        return 0;
    }

    novo = malloc(sizeof(NoLista));
    if (novo == NULL) {
        return 0;
    }

    novo->dado = dado;
    novo->prox = NULL;

    if (lista->inicio == NULL) {
        lista->inicio = novo;
    } else {
        NoLista *ultimo = lista->inicio;

        while (ultimo->prox != NULL) {
            ultimo = ultimo->prox;
        }

        ultimo->prox = novo;
    }

    lista->quantidade++;
    return 1;
}

void *lista_no_indice(const Lista *lista, int indice) {
    NoLista *atual;
    int i;

    if (lista == NULL || indice < 0 || indice >= lista->quantidade) {
        return NULL;
    }

    atual = lista->inicio;
    for (i = 0; i < indice; i++) {
        atual = atual->prox;
    }

    return atual->dado;
}

void *lista_busca(const Lista *lista, PredicadoLista pred, void *ctx) {
    NoLista *atual;

    if (lista == NULL || pred == NULL) {
        return NULL;
    }

    atual = lista->inicio;
    while (atual != NULL) {
        if (pred(atual->dado, ctx)) {
            return atual->dado;
        }

        atual = atual->prox;
    }

    return NULL;
}

int lista_remove(Lista *lista, PredicadoLista pred, void *ctx,
                 LiberaDadoLista libera_dado) {
    NoLista *atual;
    NoLista *anterior;

    if (lista == NULL || pred == NULL) {
        return 0;
    }

    atual = lista->inicio;
    anterior = NULL;

    while (atual != NULL) {
        if (pred(atual->dado, ctx)) {
            if (anterior == NULL) {
                lista->inicio = atual->prox;
            } else {
                anterior->prox = atual->prox;
            }

            if (libera_dado != NULL) {
                libera_dado(atual->dado);
            }

            free(atual);
            lista->quantidade--;
            return 1;
        }

        anterior = atual;
        atual = atual->prox;
    }

    return 0;
}
