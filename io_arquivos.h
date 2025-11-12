#ifndef IO_ARQUIVOS_H
#define IO_ARQUIVOS_H

#include "estruturas.h"

// Funções de leitura de cabeçalhos
int lerCabecalhoPessoa(FILE *arquivo, CabecalhoPessoa *cabecalho);
int lerCabecalhoIndice(FILE *arquivo, CabecalhoIndice *cabecalho);
int lerCabecalhoSegue(FILE *arquivo, CabecalhoSegue *cabecalho);

// Funções de escrita de cabeçalhos
int escreverCabecalhoPessoa(FILE *arquivo, CabecalhoPessoa *cabecalho);
int escreverCabecalhoIndice(FILE *arquivo, CabecalhoIndice *cabecalho);
int escreverCabecalhoSegue(FILE *arquivo, CabecalhoSegue *cabecalho);

// Funções de inicialização de cabeçalhos
void inicializarCabecalhoPessoa(CabecalhoPessoa *cabecalho);
void inicializarCabecalhoIndice(CabecalhoIndice *cabecalho);
void inicializarCabecalhoSegue(CabecalhoSegue *cabecalho);

// Funções de validação de arquivos
int validarArquivoConsistente(char status);
void marcarArquivoInconsistente(FILE *arquivo, int tipo); // tipo: 0=Pessoa, 1=Indice, 2=Segue
void marcarArquivoConsistente(FILE *arquivo, int tipo);

// Funções auxiliares de arquivo
FILE* abrirArquivoBinario(char *nomeArquivo, char *modo);
FILE* criarArquivoBinario(char *nomeArquivo);

#endif

