#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bd_partidas.h"
#include "bd_times.h"

#define ARQ_TIMES "times.csv"
#define ARQ_PARTIDAS "partidas.csv"
#define ARQ_CLASSIF "bd_classificacao.csv"
#define TAM_ENTRADA 100

static void ler_linha(char *buf, int tam) {
    if (fgets(buf, tam, stdin) == NULL) {
        buf[0] = '\0';
        return;
    }
    buf[strcspn(buf, "\r\n")] = '\0';
}

static void menu(void) {
    printf("\nSistema de Gerenciamento de Partidas\n");
    printf("1 - Consultar time\n");
    printf("2 - Consultar partidas\n");
    printf("3 - Atualizar partida (Parte 2)\n");
    printf("4 - Remover partida (Parte 2)\n");
    printf("5 - Inserir partida (Parte 2)\n");
    printf("6 - Imprimir tabela de classificação\n");
    printf("Q - Sair\n");
    printf("Opcao: ");
}

static void consulta_time(BDTimes *times, BDPartidas *partidas) {
    char prefixo[TAM_ENTRADA];

    atualiza_pontuacao(times, partidas);
    printf("Digite o nome ou prefixo do time: ");
    ler_linha(prefixo, sizeof(prefixo));
    busca_time_prefixo(times, prefixo);
}

static void consulta_partidas(BDPartidas *partidas, BDTimes *times) {
    char buf[TAM_ENTRADA];
    char prefixo[TAM_ENTRADA];
    int modo;

    printf("Escolha o modo de consulta:\n");
    printf("1 - Por time mandante\n");
    printf("2 - Por time visitante\n");
    printf("3 - Por time mandante ou visitante\n");
    printf("4 - Retornar ao menu principal\n");
    printf("Opcao: ");
    ler_linha(buf, sizeof(buf));

    if (strcmp(buf, "4") == 0) {
        return;
    }

    modo = atoi(buf);
    if (modo < 1 || modo > 3) {
        printf("Modo de consulta invalido.\n");
        return;
    }

    printf("Digite o nome: ");
    ler_linha(prefixo, sizeof(prefixo));
    busca_partidas(partidas, times, modo, prefixo);
}

static void imprime_tabela(BDTimes *times, BDPartidas *partidas) {
    atualiza_pontuacao(times, partidas);
    mostra_classificacao(times);
    grava_classificacao_csv(times, ARQ_CLASSIF);
}

int main(void) {
    BDTimes *times = cria_times_bd();
    BDPartidas *partidas = cria_partidas_bd();
    char opcao[TAM_ENTRADA];

    if (times == NULL || partidas == NULL) {
        fprintf(stderr, "Erro ao alocar estruturas do sistema.\n");
        libera_times_bd(times);
        libera_partidas_bd(partidas);
        return 1;
    }

    if (!carrega_times(times, ARQ_TIMES)) {
        fprintf(stderr, "Erro ao carregar %s\n", ARQ_TIMES);
        libera_times_bd(times);
        libera_partidas_bd(partidas);
        return 1;
    }

    if (!carrega_partidas(partidas, ARQ_PARTIDAS)) {
        fprintf(stderr, "Erro ao carregar %s\n", ARQ_PARTIDAS);
        libera_times_bd(times);
        libera_partidas_bd(partidas);
        return 1;
    }

    atualiza_pontuacao(times, partidas);

    while (1) {
        menu();
        ler_linha(opcao, sizeof(opcao));

        if (strcmp(opcao, "Q") == 0 || strcmp(opcao, "q") == 0) {
            break;
        }

        if (strcmp(opcao, "1") == 0) {
            consulta_time(times, partidas);
        } else if (strcmp(opcao, "2") == 0) {
            consulta_partidas(partidas, times);
        } else if (strcmp(opcao, "3") == 0 || strcmp(opcao, "4") == 0 ||
                   strcmp(opcao, "5") == 0) {
            printf("Opcao da Parte 2 - ainda nao implementada.\n");
        } else if (strcmp(opcao, "6") == 0) {
            imprime_tabela(times, partidas);
        } else {
            printf("Opcao invalida.\n");
        }
    }

    libera_times_bd(times);
    libera_partidas_bd(partidas);
    return 0;
}
