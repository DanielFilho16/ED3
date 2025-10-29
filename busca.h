#ifndef BUSCA_H
#define BUSCA_H

#include "estruturas.h"


// Funções de busca modularizadas
ResultadoBusca* buscarPessoas(char *binArquivo, char *indiceArquivo, char *nomeCampo, char *valorBusca);
void liberarResultadoBusca(ResultadoBusca *resultado);


// Funções auxiliares de busca
int buscaSequencial(char *binArquivo, char *nomeCampo, char *valorBusca);
int verificarMatch(RegistroPessoa *pessoa, char *nomeCampo, char *valorBusca);


int BuscaBinariaSegue(RegistroSegue **registros, int tamanho, int idPessoaQueSegue);

// Função auxiliar para busca binária no índice
int buscaBinariaIndice(char *indiceArquivo, int idPessoa, long long *byteOffset);


// Funções auxiliares para JOIN
int buscaBinariaSegue(FILE *arquivo, int idPessoaQueSegue, RegistroSegue *resultados, int *quantidadeResultados);
int buscarPessoaParaJoin(char *binArquivo, char *nomeCampo, char *valorBusca, RegistroPessoa *pessoa);


int buscarEMarcarRemocao(char *binArquivo, char *nomeCampo, char *valorBusca);


#endif
