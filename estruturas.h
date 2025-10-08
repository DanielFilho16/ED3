#ifndef ESTRUTURAS_H
#define ESTRUTURAS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

// Estrutura do cabeçalho do arquivo pessoa.bin
typedef struct {
    char status;                // 1 byte
    int quantidadePessoas;      // 4 bytes
    int quantidadeRemovidos;    // 4 bytes
    long long proxByteOffset;   // 8 bytes
} CabecalhoPessoa;

// Estrutura do registro pessoa
typedef struct {
    char removido;              // 1 byte
    int tamanhoRegistro;        // 4 bytes
    int idPessoa;              // 4 bytes
    int idadePessoa;           // 4 bytes
    int tamanhoNomePessoa;     // 4 bytes
    char *nomePessoa;          // variável
    int tamanhoNomeUsuario;    // 4 bytes
    char *nomeUsuario;         // variável
} RegistroPessoa;

// Estrutura do cabeçalho do índice primário (indexaPessoa.bin)
typedef struct {
    char status;               // 1 byte
    char lixo[11];            // 11 bytes de lixo ($) para completar 12 bytes
} CabecalhoIndice;

// Estrutura do registro do índice
typedef struct {
    int idPessoa;             // 4 bytes
    //int byteOffset;           // 4 bytes
    long byteOffset;
} RegistroIndice;

// Declaração das funções
void CREAT_INDEX(char *nomeArquivo);
void CREAT_TABLE(char *csvArquivo, char *binArquivo, char *indiceArquivo);
void SELECT(char *binArquivo);
void SELECT_WHERE(char *binArquivo, char *indiceArquivo, int n);
void INSERT_INTERACTIVE(char *binArquivo, char *indiceArquivo);

// Função auxiliar para leitura de registros
int lerRegistroPessoa(FILE *arquivo, RegistroPessoa *pessoa);

// Funções do arquivo utilidades.c
void binarioNaTela(char *nomeArquivoBinario);
void scan_quote_string(char *str);

#endif

