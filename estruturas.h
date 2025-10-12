#ifndef ESTRUTURAS_H
#define ESTRUTURAS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

// cabecalho do arquivo pessoa.bin
typedef struct {
    char status;                // 1 byte
    int quantidadePessoas;      // 4 bytes
    int quantidadeRemovidos;    // 4 bytes
    long long proxByteOffset;   // 8 bytes
} CabecalhoPessoa;

// estrutura do registro pessoa
typedef struct {
    char removido;              // 1 byte
    int tamanhoRegistro;        // 4 bytes
    int idPessoa;              // 4 bytes
    int idadePessoa;           // 4 bytes
    int tamanhoNomePessoa;     // 4 bytes
    char *nomePessoa;          // vari�vel
    int tamanhoNomeUsuario;    // 4 bytes
    char *nomeUsuario;         // vari�vel
} RegistroPessoa;

// estrutura do indice
typedef struct {
    char status;               // 1 byte
    char lixo[11];            // 11 bytes de lixo 
} CabecalhoIndice;

// Estrutura do registro do indice
typedef struct {
    int idPessoa;             // 4 bytes
    long long byteOffset;     //8 bytes
} RegistroIndice;

//funções
void CREAT_INDEX(char *nomeArquivo);
void CREAT_TABLE(char *csvArquivo, char *binArquivo, char *indiceArquivo);
void SELECT(char *binArquivo);
void SELECT_WHERE(char *binArquivo, char *indiceArquivo, int n);

// funções auxiliares
int lerRegistroPessoa(FILE *arquivo, RegistroPessoa *pessoa);
void binarioNaTela(char *nomeArquivoBinario);
void scan_quote_string(char *str);

#endif

