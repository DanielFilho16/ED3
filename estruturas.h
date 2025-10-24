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

// Cabeçalho do arquivo segue.bin
typedef struct {
    char status;                // 1 byte
    int quantidadeSegue;        // 4 bytes
    int quantidadeRemovidos;    // 4 bytes
    long long proxByteOffset;   // 8 bytes
} CabecalhoSegue;

// Estrutura do registro segue
typedef struct {
    char removido;              // 1 byte
    int tamanhoRegistro;        // 4 bytes
    int idPessoaQueSegue;       // 4 bytes
    int idPessoaQueESeguida;    // 4 bytes
    int tamanhoDataInicio;      // 4 bytes
    char *dataInicioQueSegue;   // variável
    int tamanhoDataFim;         // 4 bytes
    char *dataFimQueSegue;      // variável
    int grauAmizade;            // 4 bytes
} RegistroSegue;

//funções
void CREAT_INDEX(char *nomeArquivo);
void CREAT_TABLE(char *csvArquivo, char *binArquivo, char *indiceArquivo);
void SELECT(char *binArquivo);
void SELECT_WHERE(char *binArquivo, char *indiceArquivo, int n);

// Novas funcionalidades
void INSERT_INTO(char *binArquivo, char *indiceArquivo);
void DELETE_FROM(char *binArquivo, char *indiceArquivo);
void UPDATE(char *binArquivo, char *indiceArquivo);
void CREAT_TABLE_SEGUE(char *csvArquivo, char *binArquivo);
void SELECT_FROM_SEGUE(char *binArquivo, int n);
void JOIN(char *arquivoPessoa, char *arquivoIndice, char *arquivoSegue, int n);
void ORDER_BY(char *arquivoDesordenado, char *arquivoOrdenado);

// Funções auxiliares para DELETE
int buscarEMarcarRemocao(char *binArquivo, char *nomeCampo, char *valorBusca);

// Funções auxiliares para INSERT
int verificarIdExistente(char *binArquivo, int idPessoa);
int inserirRegistroPessoa(char *binArquivo, RegistroPessoa *pessoa);
void atualizarIndice(char *indiceArquivo, int idPessoa, long long byteOffset);

// funções auxiliares
int lerRegistroPessoa(FILE *arquivo, RegistroPessoa *pessoa);
void binarioNaTela(char *nomeArquivoBinario);
void scan_quote_string(char *str);

// Funções auxiliares para segue
int calcularTamanhoRegistroSegue(RegistroSegue *segue);
void escreverRegistroSegue(FILE *arquivo, RegistroSegue *segue);
int ler_csv_segue(FILE *csv, RegistroSegue *segue);
int lerRegistroSegue(FILE *arquivo, RegistroSegue *segue);
int compararRegistrosSegue(const void *a, const void *b);

// Funções auxiliares para JOIN
int buscaBinariaSegue(FILE *arquivo, int idPessoaQueSegue, RegistroSegue *resultados, int *quantidadeResultados);
int buscarPessoaParaJoin(char *binArquivo, char *nomeCampo, char *valorBusca, RegistroPessoa *pessoa);

#endif

