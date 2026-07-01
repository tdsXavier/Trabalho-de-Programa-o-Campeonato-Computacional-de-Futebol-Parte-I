# Campeonato Computacional de Futebol — Parte II

![Linguagem](https://img.shields.io/badge/Linguagem-C-blue.svg)
![Status](https://img.shields.io/badge/Status-Parte%20II-brightgreen.svg)

**Disciplina:** Estrutura de Dados  
**IFES — Campus Serra**  
**Professor:** Thiago M. Paixão

| Aluno |
|-------|
| André Vitor Caillaux Bacelar Andrade |
| Tiago dos Santos Xavier |

---

## Objetivo

Sistema em C para gerenciar times e partidas de um campeonato em pontos corridos. Os dados são carregados de arquivos CSV, armazenados em **listas encadeadas** na memória e manipulados por um menu no terminal.

Na Parte II foram implementadas:

- consulta de times e partidas (opções 1 e 2);
- atualização, remoção e inserção de partidas (opções 3, 4 e 5);
- impressão da tabela de classificação **ordenada por mérito esportivo** (opção 6);
- persistência em `bd_partidas.csv` e `bd_classificacao.csv`.

---

## Funcionalidades principais

| Opção | Descrição |
|-------|-----------|
| 1 | Consultar time por prefixo do nome |
| 2 | Consultar partidas (mandante, visitante ou ambos) |
| 3 | Atualizar placar de uma partida existente |
| 4 | Remover partida com confirmação |
| 5 | Inserir partida com ID automático |
| 6 | Imprimir tabela de classificação ordenada |
| Q | Encerrar o programa |

**Detalhes das operações de manutenção (3, 4 e 5):**

- Antes de atualizar ou remover, o sistema executa uma **consulta de partidas** para localizar o registro.
- Na atualização, digite `-` para **manter** o placar atual de um dos times.
- Inserção, atualização e remoção exigem confirmação **S/N**.
- Após qualquer alteração confirmada, as estatísticas são recalculadas e `bd_partidas.csv` é atualizado.

---

## Critérios de classificação

Para cada time são exibidos: **ID**, **Time**, **V**, **E**, **D**, **GM**, **GS**, **S** e **PG**.

**Cálculos:**

- S = GM − GS
- PG = 3 × V + E

**Ordenação na opção 6 (decrescente por mérito):**

1. Pontos ganhos (PG)
2. Vitórias (V)
3. Saldo de gols (S)
4. ID crescente (desempate final)

A ordenação usa `qsort` sobre um vetor temporário de ponteiros, sem alterar a ordem física da lista encadeada.

---

## Estrutura do repositório

```txt
.
├── main.c
├── aplicacao.c / aplicacao.h
├── lista.c / lista.h
├── time.c / time.h
├── partida.c / partida.h
├── bd_times.c / bd_times.h
├── bd_partidas.c / bd_partidas.h
├── Makefile
├── README.md
├── .gitignore
├── times.csv
├── partidas.csv
├── partidas_vazio.csv
├── partidas_parcial.csv
├── partidas_completo.csv
├── bd_partidas.csv
└── bd_classificacao.csv
```

---

## TADs do projeto

### Time

Representa um clube com ID, nome e estatísticas acumuladas (V, E, D, GM, GS). As funções `saldo_gols()` e `calc_pontos()` calculam S e PG.

### Partida

Representa um jogo com ID, IDs dos dois times e placar (GolsTime1, GolsTime2).

### Lista (TADLinkedList)

Lista simplesmente encadeada genérica com inserção no fim, busca, remoção e liberação com callback. Reutilizada por `BDTimes` e `BDPartidas`.

### BDTimes

Gerencia a coleção de times em lista encadeada. Carrega `times.csv`, recalcula estatísticas com base nas partidas e imprime/grava a classificação ordenada.

### BDPartidas

Gerencia a coleção de partidas em lista encadeada. Oferece consulta, inserção, atualização, remoção e gravação em `bd_partidas.csv`.

### aplicacao

Camada de aplicação responsável pelo menu, leitura de entradas e fluxos de confirmação (S/N). O `main.c` apenas chama `executar_aplicacao()`.

---

## Encapsulamento

- As structs internas de **BDTimes** e **BDPartidas** ficam definidas apenas nos respectivos arquivos `.c` (tipo opaco no `.h`).
- Os arquivos `.h` expõem somente a interface pública de cada módulo.
- `Time` e `Partida` são TADs elementares expostos nos `.h`, conforme o enunciado.

---

## Persistência

| Arquivo | Função |
|---------|--------|
| `times.csv` | Cadastro inicial dos times (leitura na inicialização) |
| `partidas.csv` | Partidas iniciais (leitura na inicialização) |
| `bd_partidas.csv` | Partidas atuais após inserção, atualização ou remoção |
| `bd_classificacao.csv` | Classificação ordenada gerada na opção 6 |
| `partidas_vazio.csv` | Cenário de teste: campeonato sem jogos |
| `partidas_parcial.csv` | Cenário de teste: campeonato em andamento |
| `partidas_completo.csv` | Cenário de teste: campeonato finalizado |

Sempre que uma partida é inserida, atualizada ou removida, o programa recalcula as estatísticas dos times e atualiza `bd_partidas.csv`.

---

## Como compilar e executar

**Ambiente recomendado:** Ubuntu 22.04 (WSL) com GCC 11.

```bash
cd "/caminho/para/o/projeto"
make
./campeonato
```

Para limpar artefatos de compilação:

```bash
make clean
```

Compilação manual:

```bash
gcc -Wall -Wextra -std=c11 -o campeonato \
    main.c aplicacao.c lista.c time.c partida.c bd_times.c bd_partidas.c
```

**Passo a passo:**

1. Abra o terminal na pasta do projeto (onde estão os `.c`, o `Makefile` e os `.csv`).
2. Certifique-se de que `times.csv` e `partidas.csv` estão presentes.
3. Execute `make` e depois `./campeonato`.

**Trocar cenário de partidas:**

```bash
cp partidas_vazio.csv partidas.csv
cp partidas_parcial.csv partidas.csv
cp partidas_completo.csv partidas.csv
```

---

## Exemplo de uso

Ao iniciar, o programa exibe o menu:

```txt
Sistema de Gerenciamento de Partidas
1 - Consultar time
2 - Consultar partidas
3 - Atualizar partida
4 - Remover partida
5 - Inserir partida
6 - Imprimir tabela de classificação
Q - Sair
```

**Exemplos práticos:**

- Consultar time por prefixo: `jav` encontra `JAVAlis` (busca ignora maiúsculas/minúsculas).
- Consultar partidas do time `NET` como mandante, visitante ou em qualquer posição.
- Atualizar placar informando `-` para manter um dos valores.
- Inserir partida informando IDs dos times e placar; o sistema gera o ID da partida.
- Remover partida após consulta e confirmação S/N.

**Saída da consulta de partidas:**

```txt
ID  Time1           Time2           Placar1 Placar2
-------------------------------------------------------
2   NETunos         JAVAlis               0       2
```

**Saída da opção 6 com `partidas_completo.csv`:**

```txt
Imprimindo classificação...

ID  Time              V   E   D  GM  GS    S   PG
--------------------------------------------------
0   JAVAlis          13   3   2  58  30   28   42
2   SemCTRL          11   2   5  43  34    9   35
1   ESCorpiões       10   2   6  55  39   16   32
4   PYthons           8   2   8  47  40    7   26
9   REACTivos         8   2   8  34  38   -4   26
5   SeQueLas          7   3   8  50  49    1   24
6   NETunos           7   2   9  48  53   -5   23
3   GOrilas           7   0  11  40  50  -10   21
7   LOOPardos         6   3   9  38  55  -17   21
8   RUSTicos          3   1  14  40  65  -25   10
```

**Saída da opção 6 com `partidas_vazio.csv`:**

```txt
Imprimindo classificação...

ID  Time              V   E   D  GM  GS    S   PG
--------------------------------------------------
0   JAVAlis           0   0   0   0   0    0    0
1   ESCorpiões        0   0   0   0   0    0    0
2   SemCTRL           0   0   0   0   0    0    0
...
```

---

## Decisões de implementação

- Separação em módulos `.c`/`.h` com responsabilidade clara por TAD.
- `main.c` mínimo; menu e fluxo do sistema em `aplicacao.c`.
- Listas simplesmente encadeadas (`lista.c`) para armazenar times e partidas.
- Alocação dinâmica (`malloc`) para cada time, partida e nó da lista.
- Leitura de CSV com `fgets` e `sscanf`, sem bibliotecas externas.
- Busca por prefixo case-insensitive nas consultas de times e partidas.
- Alinhamento de tabelas com suporte a UTF-8 (ex.: `ESCorpiões`).
- Validação de entradas e confirmação S/N nas operações críticas.

---

## Testes realizados

```bash
make clean && make
cp partidas_vazio.csv partidas.csv && ./campeonato
cp partidas_parcial.csv partidas.csv && ./campeonato
cp partidas_completo.csv partidas.csv && ./campeonato
```

Em cada cenário foram testadas as opções 1, 2, 3, 4, 5 e 6, incluindo:

- atualização com `-` para manter placar;
- confirmação S/N nas operações de manutenção;
- inserção com ID automático;
- ordenação da classificação por PG, V e S.

---

## Entrega

O repositório deve conter o código-fonte, o `Makefile`, o `README.md` e os arquivos:

- `bd_partidas.csv`
- `bd_classificacao.csv`

Antes de enviar, execute a **opção 6** com o cenário desejado para gerar `bd_classificacao.csv` atualizado. Se houver alterações em partidas, confira também se `bd_partidas.csv` reflete o estado final do sistema.

---

## Ambiente de referência

```txt
Ubuntu 22.04
GCC 11
WSL (Windows Subsystem for Linux)
```
