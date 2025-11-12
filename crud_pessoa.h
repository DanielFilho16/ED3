#ifndef CRUD_PESSOA_H
#define CRUD_PESSOA_H

#include "estruturas.h"

// Operações CRUD de Pessoa
void CREAT_INDEX(char *nomeArquivo);
void CREAT_TABLE(char *csvArquivo, char *binArquivo, char *indiceArquivo);
void SELECT(char *binArquivo);
void SELECT_WHERE(char *binArquivo, char *indiceArquivo, int n);
void INSERT_INTO(char *binArquivo, char *indiceArquivo);
void DELETE_FROM(char *binArquivo, char *indiceArquivo);
void UPDATE(char *binArquivo, char *indiceArquivo);

// Funções auxiliares
int inserirRegistroPessoa(char *binArquivo, RegistroPessoa *pessoa);
void aplicarAtualizacoes(RegistroPessoa *pessoa, char *nomeCampo, char *valorAtualizacao);

#endif

