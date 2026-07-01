#include "aplicacao.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bd_partidas.h"
#include "bd_times.h"

#define ARQ_TIMES "times.csv"
#define ARQ_PARTIDAS "partidas.csv"
#define ARQ_BD_PARTIDAS "bd_partidas.csv"
#define ARQ_CLASSIF "bd_classificacao.csv"
#define TAM_ENTRADA 100

static void ler_linha(char *buf, int tam) {
    if (fgets(buf, tam, stdin) == NULL) {
        buf[0] = '\0';
        return;
    }
    buf[strcspn(buf, "\r\n")] = '\0';
}

static int ler_confirmacao_sn(void) {
    char buf[TAM_ENTRADA];

    ler_linha(buf, sizeof(buf));
    return buf[0] == 'S' || buf[0] == 's';
}

static int ler_inteiro(const char *mensagem) {
    char buf[TAM_ENTRADA];

    printf("%s", mensagem);
    ler_linha(buf, sizeof(buf));
    return atoi(buf);
}

static int ler_placar_opcional(const char *mensagem, int valor_atual) {
    char buf[TAM_ENTRADA];

    printf("%s", mensagem);
    ler_linha(buf, sizeof(buf));

    if (strcmp(buf, "-") == 0) {
        return valor_atual;
    }

    return atoi(buf);
}

static void imprime_partida_resumo(const Partida *p, const BDTimes *times) {
    const char *nome1 = nome_do_time(times, p->id_time1);
    const char *nome2 = nome_do_time(times, p->id_time2);

    imprime_linha_partida(p->id, nome1, nome2, p->gols_time1, p->gols_time2);
}

static int escolher_modo_consulta(void) {
    char buf[TAM_ENTRADA];
    int modo;

    printf("Escolha o modo de consulta:\n");
    printf("1 - Por time mandante\n");
    printf("2 - Por time visitante\n");
    printf("3 - Por time mandante ou visitante\n");
    printf("4 - Retornar ao menu principal\n");
    printf("Opcao: ");
    ler_linha(buf, sizeof(buf));

    if (strcmp(buf, "4") == 0) {
        return -1;
    }

    modo = atoi(buf);
    if (modo < 1 || modo > 3) {
        printf("Modo de consulta invalido.\n");
        return -1;
    }

    return modo;
}

static int consultar_partidas_interativo(BDPartidas *partidas, BDTimes *times) {
    char prefixo[TAM_ENTRADA];
    int modo = escolher_modo_consulta();

    if (modo < 0) {
        return 0;
    }

    printf("Digite o nome: ");
    ler_linha(prefixo, sizeof(prefixo));
    return busca_partidas(partidas, times, modo, prefixo);
}

static void persistir_mudancas(BDTimes *times, BDPartidas *partidas) {
    atualiza_pontuacao(times, partidas);
    grava_partidas_csv(partidas, ARQ_BD_PARTIDAS);
}

static void exibir_menu(void) {
    printf("\nSistema de Gerenciamento de Partidas\n");
    printf("1 - Consultar time\n");
    printf("2 - Consultar partidas\n");
    printf("3 - Atualizar partida\n");
    printf("4 - Remover partida\n");
    printf("5 - Inserir partida\n");
    printf("6 - Imprimir tabela de classificação\n");
    printf("Q - Sair\n");
    printf("Opcao: ");
}

static void opcao_consultar_time(BDTimes *times, BDPartidas *partidas) {
    char prefixo[TAM_ENTRADA];

    atualiza_pontuacao(times, partidas);
    printf("Digite o nome ou prefixo do time: ");
    ler_linha(prefixo, sizeof(prefixo));
    busca_time_prefixo(times, prefixo);
}

static void opcao_consultar_partidas(BDPartidas *partidas, BDTimes *times) {
    consultar_partidas_interativo(partidas, times);
}

static void opcao_atualizar_partida(BDTimes *times, BDPartidas *partidas) {
    Partida *p;
    Partida proposta;
    int id;
    int g1;
    int g2;

    if (!consultar_partidas_interativo(partidas, times)) {
        return;
    }

    id = ler_inteiro("Digite o ID do registro a ser atualizado: ");
    p = busca_partida_id(partidas, id);
    if (p == NULL) {
        printf("Partida nao encontrada.\n");
        return;
    }

    printf("Digite o novo valor para os campos Placar1 e Placar2 "
           "(para manter o valor atual de um campo, digite '-'): \n");
    g1 = ler_placar_opcional("Placar1: ", p->gols_time1);
    g2 = ler_placar_opcional("Placar2: ", p->gols_time2);

    if (g1 < 0 || g2 < 0) {
        printf("Placar invalido.\n");
        return;
    }

    proposta = *p;
    proposta.gols_time1 = g1;
    proposta.gols_time2 = g2;

    printf("\n");
    imprime_cabecalho_partidas();
    imprime_partida_resumo(&proposta, times);
    printf("Confirma os novos valores para o registro abaixo? (S/N)\n");
    if (!ler_confirmacao_sn()) {
        printf("Atualizacao cancelada.\n");
        return;
    }

    if (!atualiza_placar_partida(partidas, id, g1, g2)) {
        printf("Erro ao atualizar partida.\n");
        return;
    }

    persistir_mudancas(times, partidas);
    printf("Registro atualizado com sucesso.\n");
}

static void opcao_remover_partida(BDTimes *times, BDPartidas *partidas) {
    Partida *p;
    int id;

    if (!consultar_partidas_interativo(partidas, times)) {
        return;
    }

    id = ler_inteiro("Digite o ID do registro a ser removido: ");
    p = busca_partida_id(partidas, id);
    if (p == NULL) {
        printf("Partida nao encontrada.\n");
        return;
    }

    printf("\n");
    imprime_cabecalho_partidas();
    imprime_partida_resumo(p, times);
    printf("Tem certeza de que deseja excluir o registro abaixo? (S/N)\n");
    if (!ler_confirmacao_sn()) {
        printf("Remocao cancelada.\n");
        return;
    }

    if (!remove_partida_id(partidas, id)) {
        printf("Erro ao remover partida.\n");
        return;
    }

    persistir_mudancas(times, partidas);
    printf("Registro removido com sucesso.\n");
}

static void opcao_inserir_partida(BDTimes *times, BDPartidas *partidas) {
    Partida proposta;
    int t1;
    int t2;
    int g1;
    int g2;

    printf("Para inserir um novo registro, digite os valores para "
           "os campos Time1, Time2, Placar1 e Placar2:\n");
    t1 = ler_inteiro("Time1: ");
    t2 = ler_inteiro("Time2: ");
    g1 = ler_inteiro("Placar1: ");
    g2 = ler_inteiro("Placar2: ");

    if (acha_time_id(times, t1) == NULL || acha_time_id(times, t2) == NULL) {
        printf("Time informado nao existe.\n");
        return;
    }

    if (g1 < 0 || g2 < 0) {
        printf("Placar invalido.\n");
        return;
    }

    proposta.id = proximo_id_partida(partidas);
    proposta.id_time1 = t1;
    proposta.id_time2 = t2;
    proposta.gols_time1 = g1;
    proposta.gols_time2 = g2;

    printf("\n");
    imprime_cabecalho_partidas();
    imprime_partida_resumo(&proposta, times);
    printf("Confirma a insercao do registro abaixo? (S/N)\n");
    if (!ler_confirmacao_sn()) {
        printf("Insercao cancelada.\n");
        return;
    }

    if (!insere_partida(partidas, t1, t2, g1, g2)) {
        printf("Erro ao inserir partida.\n");
        return;
    }

    persistir_mudancas(times, partidas);
    printf("O registro foi inserido com sucesso.\n");
}

static void opcao_imprimir_tabela(BDTimes *times, BDPartidas *partidas) {
    atualiza_pontuacao(times, partidas);
    mostra_classificacao(times);
    grava_classificacao_csv(times, ARQ_CLASSIF);
}

static int executar_menu(BDTimes *times, BDPartidas *partidas) {
    char opcao[TAM_ENTRADA];

    while (1) {
        exibir_menu();
        ler_linha(opcao, sizeof(opcao));

        if (strcmp(opcao, "Q") == 0 || strcmp(opcao, "q") == 0) {
            return 0;
        }

        if (strcmp(opcao, "1") == 0) {
            opcao_consultar_time(times, partidas);
        } else if (strcmp(opcao, "2") == 0) {
            opcao_consultar_partidas(partidas, times);
        } else if (strcmp(opcao, "3") == 0) {
            opcao_atualizar_partida(times, partidas);
        } else if (strcmp(opcao, "4") == 0) {
            opcao_remover_partida(times, partidas);
        } else if (strcmp(opcao, "5") == 0) {
            opcao_inserir_partida(times, partidas);
        } else if (strcmp(opcao, "6") == 0) {
            opcao_imprimir_tabela(times, partidas);
        } else {
            printf("Opcao invalida.\n");
        }
    }
}

int executar_aplicacao(void) {
    BDTimes *times = cria_times_bd();
    BDPartidas *partidas = cria_partidas_bd();

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
    executar_menu(times, partidas);

    libera_times_bd(times);
    libera_partidas_bd(partidas);
    return 0;
}
