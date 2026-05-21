Trabalho Prático — Campeonato de Futebol (Parte I)

Aluno: André Vitor caillaux Bacelar Andrade e Tiago dos Santos Xavier
Disciplina: Estrutura de Dados
IFES — Campus Serra



Sobre o projeto

Sistema em C que gerencia times e partidas de um campeonato em pontos corridos. Os dados ficam em arquivos CSV e são carregados em memória usando TADs (Time, Partida, BDTimes e BDPartidas).

Nesta Parte 1 implementei:





consulta de time por prefixo do nome (opção 1);



consulta de partidas (opção 2);



impressão da tabela de classificação (opção 6);



sair (Q).

As opções 3, 4 e 5 aparecem no menu, mas ficam para a Parte 2.



Ambiente que usei

Desenvolvi no Windows, porém compilei e testei no Ubuntu pelo WSL.

Ferramentas usadas no WSL:





gcc (padrão C11)



make



Arquivos do projeto





main.c — menu principal



time.c / time.h — TAD Time



partida.c / partida.h — TAD Partida



bd_times.c / bd_times.h — TAD BDTimes



bd_partidas.c / bd_partidas.h — TAD BDPartidas



Makefile



times.csv — cadastro dos times



partidas.csv — partidas que o programa lê ao executar



partidas_vazio.csv, partidas_parcial.csv, partidas_completo.csv — cenários de teste



bd_partidas.csv — cópia das partidas para entrega



bd_classificacao.csv — tabela gerada na opção 6



Como compilar

No terminal, dentro da pasta do projeto:

make

Para limpar:

make clean

Compilação manual (se precisar):

gcc -Wall -Wextra -std=c11 -o campeonato main.c time.c partida.c bd_times.c bd_partidas.c



Como executar





Abra o terminal na pasta do projeto (onde estão os .c, o Makefile e os .csv).



Deixe times.csv e partidas.csv nessa pasta.



Rode:

./campeonato

Para testar outro cenário de partidas:

cp partidas_vazio.csv partidas.csv
# ou partidas_parcial.csv / partidas_completo.csv
./campeonato

Antes de enviar o trabalho, rode a opção 6 uma vez para gerar o bd_classificacao.csv. Também deixei o bd_partidas.csv como cópia do arquivo de partidas usado nos testes.



Menu







Opção



Função





1



Consultar time





2



Consultar partidas





3



Atualizar partida (Parte 2)





4



Remover partida (Parte 2)





5



Inserir partida (Parte 2)





6



Imprimir tabela de classificação





Q



Sair

Na consulta de times e partidas, a busca é por prefixo e não diferencia maiúsculas de minúsculas. Exemplo: java, JAVA e Java encontram JAVAlis.



TADs (resumo)

Time — guarda id, nome, vitórias, empates, derrotas, gols marcados e sofridos. Saldo e pontos são calculados nas funções saldo_gols() e calc_pontos().

BDTimes — carrega times.csv, atualiza estatísticas com base nas partidas (atualiza_pontuacao), faz busca por prefixo e imprime a classificação.

Partida — id, dois times e placar.

BDPartidas — carrega partidas.csv e lista jogos filtrando por mandante, visitante ou ambos.

O main.c só chama o menu e repassa para esses módulos.



Cálculo da classificação

Para cada partida:





vitória: mais gols que o adversário;



empate: gols iguais;



derrota: menos gols.

Pontuação: PG = 3×V + E
Saldo: S = GM − GS

A tabela da Parte I é impressa na ordem do ID do time (0 a 9), sem ordenar por pontos.



Decisões que tomei





Separei o código em .c e .h para cada TAD, com main.c só no menu.



Usei alocação dinâmica (malloc) para os vetores de times e partidas.



Li os CSV com fgets e sscanf, sem biblioteca externa.



Recalculo todas as estatísticas ao consultar time ou imprimir a tabela, para não ficar valor desatualizado.



A busca por prefixo ignora maiúsculas/minúsculas, para funcionar mesmo digitando java em vez de JAVAlis.



O bd_classificacao.csv só é gravado na opção 6.



Dificuldades





Os CSV de teste usam colunas Time1ID e Time2ID; adaptei o sscanf para esse formato.



BDTimes e BDPartidas dependem um do outro (nomes nas consultas e cálculo de pontos); organizei os includes para compilar sem erro.



Testar no WSL foi necessário porque no Windows o caminho da pasta tem espaço e acento.



Testes que fiz

cp partidas_vazio.csv partidas.csv && ./campeonato
cp partidas_parcial.csv partidas.csv && ./campeonato
cp partidas_completo.csv partidas.csv && ./campeonato

Em cada teste usei as opções 1, 2 e 6 e saí com Q.
