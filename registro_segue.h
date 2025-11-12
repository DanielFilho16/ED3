#ifndef REGISTRO_SEGUE_H
#define REGISTRO_SEGUE_H

#include "estruturas.h"

// Funções de leitura/escrita de registros
int lerRegistroSegue(FILE *arquivo, RegistroSegue *segue);
void escreverRegistroSegue(FILE *arquivo, RegistroSegue *segue);

// Funções de comparação e ordenação
int compararRegistrosSegue(const void *a, const void *b);

// Funções auxiliares
void inicializarRegistroSegue(RegistroSegue *segue);
void imprimirRegistroSegue(RegistroSegue *segue);

// Funções de carregamento em memória
RegistroSegue** LerRegistrosSegue(char *nomeArquivo, int *tamanho);
void LiberarRegistrosSegue(RegistroSegue **registros, int tamanho);

#endif

