#ifndef GRAFO_H
#define GRAFO_H

#include "estruturas.h"

// Nó da lista de adjacências (aresta)
typedef struct NoAresta {
    char *nomeUsuarioDestino;  // Nome do usuário seguido
    char dataInicio[11];        // Data de início (10 chars + '\0')
    char dataFim[11];            // Data de fim (10 chars + '\0')
    char grauAmizade;            // Grau de amizade
    struct NoAresta *proximo;    // Próximo nó na lista
} NoAresta;

// Vértice do grafo (pessoa)
typedef struct Vertice {
    char *nomeUsuario;          // Nome do usuário (chave única)
    int idPessoa;                // ID da pessoa
    NoAresta *listaAdjacencias;  // Lista de pessoas seguidas
} Vertice;

// Grafo (vetor de vértices)
typedef struct {
    Vertice *vertices;           // Vetor de vértices
    int quantidadeVertices;      // Quantidade de vértices
    int capacidade;              // Capacidade do vetor
} Grafo;

// Funções principais
void funcionalidade11(char *arquivoPessoa, char *arquivoIndice, char *arquivoSegue);
void funcionalidade12(char *arquivoPessoa, char *arquivoIndice, char *arquivoSegue);
void funcionalidade13(char *arquivoPessoa, char *arquivoIndice, char *arquivoSegue, char *nomeCelebridade);
void funcionalidade14(char *arquivoPessoa, char *arquivoIndice, char *arquivoSegue, char *nomeFofoqueiro);

// Funções auxiliares
Grafo* criarGrafo(int capacidadeInicial);
void liberarGrafo(Grafo *grafo);
NoAresta* criarNoAresta(char *nomeUsuarioDestino, char *dataInicio, char *dataFim, char grauAmizade);
void liberarListaAdjacencias(NoAresta *lista);
void inserirArestaOrdenada(Vertice *vertice, NoAresta *novaAresta);
int compararArestas(NoAresta *a, NoAresta *b);
Vertice* buscarOuCriarVertice(Grafo *grafo, char *nomeUsuario, int idPessoa);
void ordenarVerticesPorNome(Grafo *grafo);
int compararVertices(const void *a, const void *b);
void imprimirGrafo(Grafo *grafo);

#endif

