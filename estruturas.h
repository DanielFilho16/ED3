#ifndef ESTRUTURAS_H
#define ESTRUTURAS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

// Estrutura do cabe�alho do arquivo pessoa.bin
//checado
typedef struct {
    char status;                // 1 byte
    int quantidadePessoas;      // 4 bytes
    int quantidadeRemovidos;    // 4 bytes
    long long proxByteOffset;   // 8 bytes
} CabecalhoPessoa;

// Estrutura do registro pessoa
//checado
typedef struct {
    char removido;              // 1 byte-
    int tamanhoRegistro;        // 4 bytes-
    int idPessoa;              // 4 bytes -
    int idadePessoa;           // 4 bytes-
    int tamanhoNomePessoa;     // 4 bytes-
    char *nomePessoa;          // vari�vel-
    int tamanhoNomeUsuario;    // 4 bytes-
    char *nomeUsuario;         // vari�vel-
} RegistroPessoa;

// Estrutura do cabe�alho do �ndice prim�rio (indexaPessoa.bin)
typedef struct {
    char status;               // 1 byte
    char lixo[11];            // 11 bytes de lixo ($) para completar 12 bytes
} CabecalhoIndice;

// Estrutura do registro do �ndice
typedef struct {
    int idPessoa;             // 4 bytes
    long long byteOffset;     //8 bytes
} RegistroIndice;

// Declara��o das fun��es
void CREAT_INDEX(char *nomeArquivo);
void CREAT_TABLE(char *csvArquivo, char *binArquivo, char *indiceArquivo);
void SELECT(char *binArquivo);
void SELECT_WHERE(char *binArquivo, char *indiceArquivo, int n);

// Fun��o auxiliar para leitura de registros
int lerRegistroPessoa(FILE *arquivo, RegistroPessoa *pessoa);

// Fun��es do arquivo utilidades.c
void binarioNaTela(char *nomeArquivoBinario);
void scan_quote_string(char *str);

// Funo auxiliar para ler CSV usando scan_quote_string
int ler_csv(FILE *csv, RegistroPessoa *pessoa);

#endif

