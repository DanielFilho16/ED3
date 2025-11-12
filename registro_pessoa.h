#ifndef REGISTRO_PESSOA_H
#define REGISTRO_PESSOA_H

#include "estruturas.h"

// Funções de leitura/escrita de registros
int lerRegistroPessoa(FILE *arquivo, RegistroPessoa *pessoa);
void escreverRegistroPessoa(FILE *arquivo, RegistroPessoa *pessoa);

// Funções auxiliares de registro
int calcularTamanhoRegistro(RegistroPessoa *pessoa);
void inicializarRegistroPessoa(RegistroPessoa *pessoa);
void liberarRegistroPessoa(RegistroPessoa *pessoa);
int copiarRegistroPessoa(RegistroPessoa *destino, RegistroPessoa *origem);

// Funções de impressão
void imprimirRegistroPessoa(RegistroPessoa *pessoa);

// Funções de manipulação em arquivo
int marcarRegistroRemovido(FILE *arquivo, long long posicao);
int lerRegistroEmPosicao(FILE *arquivo, long long posicao, RegistroPessoa *pessoa);
void escreverRegistroEmPosicao(FILE *arquivo, long long posicao, RegistroPessoa *pessoa);
void preencherComLixo(FILE *arquivo, long long posicao, int tamanho);

#endif

