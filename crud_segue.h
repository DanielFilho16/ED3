#ifndef CRUD_SEGUE_H
#define CRUD_SEGUE_H

#include "estruturas.h"

// Operações relacionadas a Segue
void CREAT_TABLE_SEGUE(char *csvArquivo, char *binArquivo);
void ORDER_BY(char *arquivoDesordenado, char *arquivoOrdenado);
void JOIN(char *arquivoPessoa, char *arquivoIndice, char *arquivoSegue, int n);

#endif

