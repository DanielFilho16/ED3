#ifndef UTILS_H
#define UTILS_H

#include "estruturas.h"

// Função para imprimir o binário na tela (dada pela professora)
void binarioNaTela(char *nomeArquivoBinario);

// Função para ler um campo string delimitado entre aspas (dada pela professora)
void scan_quote_string(char *str);

// Função auxiliar para remover aspas de uma string
void removerAspas(char *str);

// Função auxiliar para pular número da linha em entrada
char* pularNumeroLinha(char *entrada);

#endif

