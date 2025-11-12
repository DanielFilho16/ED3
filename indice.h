#ifndef INDICE_H
#define INDICE_H

#include "estruturas.h"

// Função de comparação para ordenação
int compararIdPessoa(const void *a, const void *b);

// Funções de criação e gerenciamento de índice
void criarIndice(char *nomeArquivo);

// Funções de carregamento e manipulação
RegistroIndice* CarregarIndiceEmMemoria(char *nomeArquivo, int *tamanho);
void atualizarIndice(char *indiceArquivo, int idPessoa, long long byteOffset);
void RemoverDoIndice(RegistroIndice **registros, int *tamanho, int idPessoa);
void ReescreverIndice(char *nomeArquivo, RegistroIndice *registros, int tamanho);

#endif

