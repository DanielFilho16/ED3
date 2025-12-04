// Dupla 31
// Aluno: Daniel Dias Silva Filho ------------ N USP: 13677114
// Aluno: Kaue Antonio Maranzano de Araujo --- N USP: 14669684

// esse arquivo ta sendo usando pras funcionalidades do trabalho 2

#include "grafo.h"
#include "busca.h"
#include "io_arquivos.h"
#include "registro_pessoa.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Função para criar um novo nó de aresta
NoAresta* criarNoAresta(char *nomeUsuarioDestino, char *dataInicio, char *dataFim, char grauAmizade) {
    NoAresta *novo = malloc(sizeof(NoAresta));
    if (novo == NULL) return NULL;
    
    if (nomeUsuarioDestino != NULL) {
        novo->nomeUsuarioDestino = malloc(strlen(nomeUsuarioDestino) + 1);
        if (novo->nomeUsuarioDestino == NULL) {
            free(novo);
            return NULL;
        }
        strcpy(novo->nomeUsuarioDestino, nomeUsuarioDestino);
    } else {
        novo->nomeUsuarioDestino = NULL;
    }
    
    if (dataInicio != NULL) {
        strncpy(novo->dataInicio, dataInicio, 10);
        novo->dataInicio[10] = '\0';
    } else {
        novo->dataInicio[0] = '$';
        novo->dataInicio[1] = '\0';
    }
    
    if (dataFim != NULL) {
        strncpy(novo->dataFim, dataFim, 10);
        novo->dataFim[10] = '\0';
    } else {
        novo->dataFim[0] = '$';
        novo->dataFim[1] = '\0';
    }
    
    novo->grauAmizade = grauAmizade;
    novo->proximo = NULL;
    
    return novo;
}

// Função para liberar uma lista de adjacências
void liberarListaAdjacencias(NoAresta *lista) {
    NoAresta *atual = lista;
    while (atual != NULL) {
        NoAresta *proximo = atual->proximo;
        if (atual->nomeUsuarioDestino) {
            free(atual->nomeUsuarioDestino);
        }
        free(atual);
        atual = proximo;
    }
}

// Função para comparar duas arestas (para ordenação)
int compararArestas(NoAresta *a, NoAresta *b) {
    // Critério 1: nomeUsuario (crescente)
    // Tratar valores nulos: nulos vêm depois dos não-nulos
    if (a->nomeUsuarioDestino == NULL && b->nomeUsuarioDestino == NULL) {
        // Ambos nulos, continuar para próximo critério
    } else if (a->nomeUsuarioDestino == NULL) {
        return 1;  // A é nulo, vem depois
    } else if (b->nomeUsuarioDestino == NULL) {
        return -1; // B é nulo, vem depois
    } else {
        int cmp = strcmp(a->nomeUsuarioDestino, b->nomeUsuarioDestino);
        if (cmp != 0) return cmp;
    }
    
    // Critério 2: dataInicio (crescente) - desempate
    // Tratar valores nulos: nulos vêm depois dos não-nulos
    int aNulo = (a->dataInicio[0] == '$');
    int bNulo = (b->dataInicio[0] == '$');
    
    if (aNulo && bNulo) return 0;
    if (aNulo) return 1;  // A é nulo, vem depois
    if (bNulo) return -1; // B é nulo, vem depois
    
    // Comparar datas (formato: DD/MM/YYYY)
    // Comparar ano primeiro
    int anoA = (a->dataInicio[6] - '0') * 1000 + (a->dataInicio[7] - '0') * 100 +
               (a->dataInicio[8] - '0') * 10 + (a->dataInicio[9] - '0');
    int anoB = (b->dataInicio[6] - '0') * 1000 + (b->dataInicio[7] - '0') * 100 +
               (b->dataInicio[8] - '0') * 10 + (b->dataInicio[9] - '0');
    if (anoA != anoB) return anoA - anoB;
    
    // Comparar mês
    int mesA = (a->dataInicio[3] - '0') * 10 + (a->dataInicio[4] - '0');
    int mesB = (b->dataInicio[3] - '0') * 10 + (b->dataInicio[4] - '0');
    if (mesA != mesB) return mesA - mesB;
    
    // Comparar dia
    int diaA = (a->dataInicio[0] - '0') * 10 + (a->dataInicio[1] - '0');
    int diaB = (b->dataInicio[0] - '0') * 10 + (b->dataInicio[1] - '0');
    return diaA - diaB;
}

// Função para inserir aresta ordenada na lista de adjacências
void inserirArestaOrdenada(Vertice *vertice, NoAresta *novaAresta) {
    if (vertice->listaAdjacencias == NULL) {
        vertice->listaAdjacencias = novaAresta;
        return;
    }
    
    // Se a nova aresta deve vir antes da primeira
    if (compararArestas(novaAresta, vertice->listaAdjacencias) < 0) {
        novaAresta->proximo = vertice->listaAdjacencias;
        vertice->listaAdjacencias = novaAresta;
        return;
    }
    
    // Procurar posição correta
    NoAresta *atual = vertice->listaAdjacencias;
    while (atual->proximo != NULL && compararArestas(novaAresta, atual->proximo) > 0) {
        atual = atual->proximo;
    }
    
    novaAresta->proximo = atual->proximo;
    atual->proximo = novaAresta;
}

// Função para criar um grafo
Grafo* criarGrafo(int capacidadeInicial) {
    Grafo *grafo = malloc(sizeof(Grafo));
    if (grafo == NULL) return NULL;
    
    grafo->vertices = malloc(capacidadeInicial * sizeof(Vertice));
    if (grafo->vertices == NULL) {
        free(grafo);
        return NULL;
    }
    
    grafo->quantidadeVertices = 0;
    grafo->capacidade = capacidadeInicial;
    
    return grafo;
}

// Função para liberar um grafo
void liberarGrafo(Grafo *grafo) {
    if (grafo == NULL) return;
    
    for (int i = 0; i < grafo->quantidadeVertices; i++) {
        if (grafo->vertices[i].nomeUsuario) {
            free(grafo->vertices[i].nomeUsuario);
        }
        liberarListaAdjacencias(grafo->vertices[i].listaAdjacencias);
    }
    
    free(grafo->vertices);
    free(grafo);
}

// Função para buscar ou criar um vértice
Vertice* buscarOuCriarVertice(Grafo *grafo, char *nomeUsuario, int idPessoa) {
    // Buscar vértice existente
    for (int i = 0; i < grafo->quantidadeVertices; i++) {
        if (strcmp(grafo->vertices[i].nomeUsuario, nomeUsuario) == 0) {
            return &grafo->vertices[i];
        }
    }
    
    // Se não encontrou, criar novo vértice
    if (grafo->quantidadeVertices >= grafo->capacidade) {
        grafo->capacidade *= 2;
        grafo->vertices = realloc(grafo->vertices, grafo->capacidade * sizeof(Vertice));
        if (grafo->vertices == NULL) return NULL;
    }
    
    Vertice *novo = &grafo->vertices[grafo->quantidadeVertices];
    novo->nomeUsuario = malloc(strlen(nomeUsuario) + 1);
    if (novo->nomeUsuario == NULL) return NULL;
    strcpy(novo->nomeUsuario, nomeUsuario);
    novo->idPessoa = idPessoa;
    novo->listaAdjacencias = NULL;
    grafo->quantidadeVertices++;
    
    return novo;
}

// Função para comparar vértices por nomeUsuario (para ordenação)
int compararVertices(const void *a, const void *b) {
    const Vertice *vertA = (const Vertice *)a;
    const Vertice *vertB = (const Vertice *)b;
    return strcmp(vertA->nomeUsuario, vertB->nomeUsuario);
}

// Função para ordenar vértices por nomeUsuario
void ordenarVerticesPorNome(Grafo *grafo) {
    qsort(grafo->vertices, grafo->quantidadeVertices, sizeof(Vertice), compararVertices);
}

// Função para obter nome de usuário por ID
char* obterNomeUsuarioPorId(char *arquivoPessoa, char *arquivoIndice, int idPessoa) {
    long long byteOffset;
    if (!buscaBinariaIndice(arquivoIndice, idPessoa, &byteOffset)) {
        return NULL;
    }
    
    FILE *arquivo = fopen(arquivoPessoa, "rb");
    if (arquivo == NULL) return NULL;
    
    fseek(arquivo, byteOffset, SEEK_SET);
    RegistroPessoa pessoa;
    if (!lerRegistroPessoa(arquivo, &pessoa)) {
        fclose(arquivo);
        return NULL;
    }
    
    fclose(arquivo);
    
    // Verificar se a pessoa está removida
    if (pessoa.removido != '0') {
        // Liberar memória do registro pessoa
        if (pessoa.nomePessoa) free(pessoa.nomePessoa);
        if (pessoa.nomeUsuario) free(pessoa.nomeUsuario);
        return NULL;
    }
    
    char *nomeUsuario = NULL;
    if (pessoa.nomeUsuario != NULL) {
        nomeUsuario = malloc(strlen(pessoa.nomeUsuario) + 1);
        if (nomeUsuario != NULL) {
            strcpy(nomeUsuario, pessoa.nomeUsuario);
        }
    }
    
    // Liberar memória do registro pessoa
    if (pessoa.nomePessoa) free(pessoa.nomePessoa);
    if (pessoa.nomeUsuario) free(pessoa.nomeUsuario);
    
    return nomeUsuario;
}

// Função para verificar se uma data é nula
int dataNula(char *data) {
    if (data == NULL) return 1;
    // Verificar se todos os caracteres são '$' ou se o primeiro é '$'
    return (data[0] == '$');
}

// Função para formatar data para impressão
void formatarData(char *data, char *saida) {
    if (data == NULL || dataNula(data)) {
        strcpy(saida, "NULO");
    } else {
        strncpy(saida, data, 10);
        saida[10] = '\0';
    }
}

// Função para formatar grau de amizade para impressão
void formatarGrauAmizade(char grauAmizade, char *saida) {
    if (grauAmizade == '$') {
        strcpy(saida, "NULO");
    } else {
        saida[0] = grauAmizade;
        saida[1] = '\0';
    }
}

// Função auxiliar genérica para construir grafo (direto ou transposto)
// Se transposto = 0: Vértice = quem segue, Aresta = quem é seguido (funcionalidade 11)
// Se transposto = 1: Vértice = quem é seguido, Aresta = quem segue (funcionalidade 12)
static Grafo* construirGrafo(char *arquivoPessoa, char *arquivoIndice, char *arquivoSegue, int transposto) {
    FILE *arquivoSegueFile;
    CabecalhoSegue cabecalhoSegue;
    RegistroSegue registroSegue;
    Grafo *grafo = criarGrafo(100);
    
    if (grafo == NULL) {
        return NULL;
    }
    
    // Abrir arquivo segue
    arquivoSegueFile = fopen(arquivoSegue, "rb");
    if (arquivoSegueFile == NULL) {
        liberarGrafo(grafo);
        return NULL;
    }
    
    // Ler cabeçalho do arquivo segue
    if (fread(&cabecalhoSegue.status, sizeof(char), 1, arquivoSegueFile) != 1 ||
        fread(&cabecalhoSegue.quantidadeSegue, sizeof(int), 1, arquivoSegueFile) != 1 ||
        fread(&cabecalhoSegue.proxRRN, sizeof(int), 1, arquivoSegueFile) != 1) {
        fclose(arquivoSegueFile);
        liberarGrafo(grafo);
        return NULL;
    }
    
    if (cabecalhoSegue.status != '1') {
        fclose(arquivoSegueFile);
        liberarGrafo(grafo);
        return NULL;
    }
    
    // Ler todos os registros de segue e construir o grafo
    while (lerRegistroSegue(arquivoSegueFile, &registroSegue)) {
        if (registroSegue.removido == '0') {
            int idVertice, idAresta;
            
            if (transposto) {
                // Grafo transposto: Vértice = quem é seguido, Aresta = quem segue
                idVertice = registroSegue.idPessoaQueESeguida;
                idAresta = registroSegue.idPessoaQueSegue;
            } else {
                // Grafo direto: Vértice = quem segue, Aresta = quem é seguido
                idVertice = registroSegue.idPessoaQueSegue;
                idAresta = registroSegue.idPessoaQueESeguida;
            }
            
            // Obter nome do usuário do vértice
            char *nomeVertice = obterNomeUsuarioPorId(arquivoPessoa, arquivoIndice, idVertice);
            if (nomeVertice == NULL) continue;
            
            // Obter nome do usuário da aresta
            char *nomeAresta = obterNomeUsuarioPorId(arquivoPessoa, arquivoIndice, idAresta);
            if (nomeAresta == NULL) {
                free(nomeVertice);
                continue;
            }
            
            // Buscar ou criar vértice (a função já copia o nome, então podemos liberar o original)
            Vertice *vertice = buscarOuCriarVertice(grafo, nomeVertice, idVertice);
            free(nomeVertice); // Liberar após copiar no vértice
            
            if (vertice == NULL) {
                free(nomeAresta);
                continue;
            }
            
            // Criar aresta (a função já copia o nome, então podemos liberar o original)
            NoAresta *novaAresta = criarNoAresta(nomeAresta, 
                                                   registroSegue.dataInicioQueSegue,
                                                   registroSegue.dataFimQueSegue,
                                                   registroSegue.grauAmizade);
            free(nomeAresta); // Liberar após copiar na aresta
            
            if (novaAresta == NULL) {
                continue;
            }
            
            // Inserir aresta ordenada
            inserirArestaOrdenada(vertice, novaAresta);
        }
    }
    
    fclose(arquivoSegueFile);
    
    // Ordenar vértices por nomeUsuario
    ordenarVerticesPorNome(grafo);
    
    return grafo;
}

// Função auxiliar para imprimir grafo no formato especificado
static void imprimirGrafoFormatado(Grafo *grafo) {
    char dataInicioFormatada[15];
    char dataFimFormatada[15];
    char grauFormatado[10];
    int primeiroVertice = 1;
    
    for (int i = 0; i < grafo->quantidadeVertices; i++) {
        Vertice *vertice = &grafo->vertices[i];
        NoAresta *aresta = vertice->listaAdjacencias;
        
        // Se não há arestas, pular este vértice
        if (aresta == NULL) continue;
        
        // Pular linha extra ao mudar de vértice de origem (exceto no primeiro)
        if (!primeiroVertice) {
            printf("\n");
        }
        primeiroVertice = 0;
        
        while (aresta != NULL) {
            // Formatar dados
            formatarData(aresta->dataInicio, dataInicioFormatada);
            formatarData(aresta->dataFim, dataFimFormatada);
            formatarGrauAmizade(aresta->grauAmizade, grauFormatado);
            
            // Imprimir no formato: nomeUsuarioOrigem, nomeUsuarioDestino, dataInicio, dataFim, grauAmizade
            printf("%s, %s, %s, %s, %s\n", 
                   vertice->nomeUsuario ? vertice->nomeUsuario : "NULO",
                   aresta->nomeUsuarioDestino ? aresta->nomeUsuarioDestino : "NULO",
                   dataInicioFormatada,
                   dataFimFormatada,
                   grauFormatado);
            
            aresta = aresta->proximo;
        }
    }
}

// Função principal da funcionalidade 11
void funcionalidade11(char *arquivoPessoa, char *arquivoIndice, char *arquivoSegue) {
    Grafo *grafo = construirGrafo(arquivoPessoa, arquivoIndice, arquivoSegue, 0);
    
    if (grafo == NULL) {
        printf("Falha na execução da funcionalidade.\n");
        return;
    }
    
    imprimirGrafoFormatado(grafo);
    liberarGrafo(grafo);
}

// Função principal da funcionalidade 12 (Grafo Transposto)
void funcionalidade12(char *arquivoPessoa, char *arquivoIndice, char *arquivoSegue) {
    Grafo *grafo = construirGrafo(arquivoPessoa, arquivoIndice, arquivoSegue, 1);
    
    if (grafo == NULL) {
        printf("Falha na execução da funcionalidade.\n");
        return;
    }
    
    imprimirGrafoFormatado(grafo);
    liberarGrafo(grafo);
}

// Estrutura para nó da fila BFS
typedef struct NoFila {
    int indiceVertice;
    struct NoFila *proximo;
} NoFila;

// Estrutura para fila BFS
typedef struct {
    NoFila *inicio;
    NoFila *fim;
} Fila;

// Criar fila vazia
static Fila* criarFila() {
    Fila *fila = malloc(sizeof(Fila));
    if (fila == NULL) return NULL;
    fila->inicio = NULL;
    fila->fim = NULL;
    return fila;
}

// Inserir na fila
static void enfileirar(Fila *fila, int indiceVertice) {
    NoFila *novo = malloc(sizeof(NoFila));
    if (novo == NULL) return;
    novo->indiceVertice = indiceVertice;
    novo->proximo = NULL;
    
    if (fila->fim == NULL) {
        fila->inicio = novo;
        fila->fim = novo;
    } else {
        fila->fim->proximo = novo;
        fila->fim = novo;
    }
}

// Remover da fila
static int desenfileirar(Fila *fila) {
    if (fila->inicio == NULL) return -1;
    
    NoFila *temp = fila->inicio;
    int indice = temp->indiceVertice;
    fila->inicio = fila->inicio->proximo;
    
    if (fila->inicio == NULL) {
        fila->fim = NULL;
    }
    
    free(temp);
    return indice;
}

// Verificar se fila está vazia
static int filaVazia(Fila *fila) {
    return (fila->inicio == NULL);
}

// Liberar fila
static void liberarFila(Fila *fila) {
    while (!filaVazia(fila)) {
        desenfileirar(fila);
    }
    free(fila);
}

// Função para verificar se uma relação está ativa (dataFim nula)
static int relacaoAtiva(NoAresta *aresta) {
    if (aresta == NULL) return 0;
    // Relação está ativa se dataFim for nula (começa com '$')
    return (aresta->dataFim[0] == '$');
}

// Função para comparar arestas para ordenação de visita (BFS)
// Retorna < 0 se a vem antes de b, > 0 se b vem antes de a, 0 se iguais
static int compararArestasParaVisita(NoAresta *a, NoAresta *b) {
    // Critério 1: nomeUsuario (alfabético crescente)
    if (a->nomeUsuarioDestino == NULL && b->nomeUsuarioDestino == NULL) {
        // Ambos nulos, continuar
    } else if (a->nomeUsuarioDestino == NULL) {
        return 1;  // A é nulo, vem depois
    } else if (b->nomeUsuarioDestino == NULL) {
        return -1; // B é nulo, vem depois
    } else {
        int cmp = strcmp(a->nomeUsuarioDestino, b->nomeUsuarioDestino);
        if (cmp != 0) return cmp;
    }
    
    // Critério 2: menor dataInicio
    int aNulo = (a->dataInicio[0] == '$');
    int bNulo = (b->dataInicio[0] == '$');
    
    if (aNulo && bNulo) {
        // Ambos nulos, continuar para próximo critério
    } else if (aNulo) {
        return 1;  // A é nulo, vem depois
    } else if (bNulo) {
        return -1; // B é nulo, vem depois
    } else {
        // Comparar datas (formato: DD/MM/YYYY)
        // Comparar ano primeiro
        int anoA = (a->dataInicio[6] - '0') * 1000 + (a->dataInicio[7] - '0') * 100 +
                   (a->dataInicio[8] - '0') * 10 + (a->dataInicio[9] - '0');
        int anoB = (b->dataInicio[6] - '0') * 1000 + (b->dataInicio[7] - '0') * 100 +
                   (b->dataInicio[8] - '0') * 10 + (b->dataInicio[9] - '0');
        if (anoA != anoB) return anoA - anoB;
        
        // Comparar mês
        int mesA = (a->dataInicio[3] - '0') * 10 + (a->dataInicio[4] - '0');
        int mesB = (b->dataInicio[3] - '0') * 10 + (b->dataInicio[4] - '0');
        if (mesA != mesB) return mesA - mesB;
        
        // Comparar dia
        int diaA = (a->dataInicio[0] - '0') * 10 + (a->dataInicio[1] - '0');
        int diaB = (b->dataInicio[0] - '0') * 10 + (b->dataInicio[1] - '0');
        if (diaA != diaB) return diaA - diaB;
    }
    
    // Critério 3: menor dataFim
    aNulo = (a->dataFim[0] == '$');
    bNulo = (b->dataFim[0] == '$');
    
    if (aNulo && bNulo) {
        return 0; // Ambos nulos, são iguais
    } else if (aNulo) {
        return 1;  // A é nulo, vem depois
    } else if (bNulo) {
        return -1; // B é nulo, vem depois
    } else {
        // Comparar datas (formato: DD/MM/YYYY)
        int anoA = (a->dataFim[6] - '0') * 1000 + (a->dataFim[7] - '0') * 100 +
                   (a->dataFim[8] - '0') * 10 + (a->dataFim[9] - '0');
        int anoB = (b->dataFim[6] - '0') * 1000 + (b->dataFim[7] - '0') * 100 +
                   (b->dataFim[8] - '0') * 10 + (b->dataFim[9] - '0');
        if (anoA != anoB) return anoA - anoB;
        
        int mesA = (a->dataFim[3] - '0') * 10 + (a->dataFim[4] - '0');
        int mesB = (b->dataFim[3] - '0') * 10 + (b->dataFim[4] - '0');
        if (mesA != mesB) return mesA - mesB;
        
        int diaA = (a->dataFim[0] - '0') * 10 + (a->dataFim[1] - '0');
        int diaB = (b->dataFim[0] - '0') * 10 + (b->dataFim[1] - '0');
        return diaA - diaB;
    }
}

// Função para ordenar lista de arestas para visita
static NoAresta* ordenarListaArestas(NoAresta *lista) {
    if (lista == NULL || lista->proximo == NULL) return lista;
    
    // Converter lista para array
    int tamanho = 0;
    NoAresta *atual = lista;
    while (atual != NULL) {
        tamanho++;
        atual = atual->proximo;
    }
    
    NoAresta **array = malloc(tamanho * sizeof(NoAresta*));
    if (array == NULL) return lista;
    
    atual = lista;
    for (int i = 0; i < tamanho; i++) {
        array[i] = atual;
        atual = atual->proximo;
    }
    
    // Ordenar array usando bubble sort (simples para pequenas listas)
    for (int i = 0; i < tamanho - 1; i++) {
        for (int j = 0; j < tamanho - i - 1; j++) {
            if (compararArestasParaVisita(array[j], array[j + 1]) > 0) {
                NoAresta *temp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = temp;
            }
        }
    }
    
    // Reconstruir lista ordenada
    for (int i = 0; i < tamanho - 1; i++) {
        array[i]->proximo = array[i + 1];
    }
    array[tamanho - 1]->proximo = NULL;
    
    NoAresta *novaLista = array[0];
    free(array);
    return novaLista;
}

// Função para buscar vértice por nomeUsuario
static int buscarIndiceVertice(Grafo *grafo, char *nomeUsuario) {
    for (int i = 0; i < grafo->quantidadeVertices; i++) {
        if (grafo->vertices[i].nomeUsuario != NULL &&
            strcmp(grafo->vertices[i].nomeUsuario, nomeUsuario) == 0) {
            return i;
        }
    }
    return -1;
}

// Função para encontrar caminho mais curto usando BFS
static int* encontrarCaminhoBFS(Grafo *grafo, int indiceOrigem, int indiceDestino) {
    if (indiceOrigem == indiceDestino) {
        // Caminho trivial: origem = destino
        int *caminho = malloc(2 * sizeof(int));
        if (caminho == NULL) return NULL;
        caminho[0] = indiceOrigem;
        caminho[1] = -1; // Marcador de fim
        return caminho;
    }
    
    // Arrays para BFS
    int *visitado = calloc(grafo->quantidadeVertices, sizeof(int));
    int *pai = malloc(grafo->quantidadeVertices * sizeof(int));
    if (visitado == NULL || pai == NULL) {
        if (visitado) free(visitado);
        if (pai) free(pai);
        return NULL;
    }
    
    for (int i = 0; i < grafo->quantidadeVertices; i++) {
        pai[i] = -1;
    }
    
    Fila *fila = criarFila();
    if (fila == NULL) {
        free(visitado);
        free(pai);
        return NULL;
    }
    
    visitado[indiceOrigem] = 1;
    enfileirar(fila, indiceOrigem);
    
    while (!filaVazia(fila)) {
        int atual = desenfileirar(fila);
        
        // Se chegamos ao destino, construir caminho
        if (atual == indiceDestino) {
            // Construir caminho de trás para frente
            int *caminho = malloc((grafo->quantidadeVertices + 1) * sizeof(int));
            if (caminho == NULL) {
                liberarFila(fila);
                free(visitado);
                free(pai);
                return NULL;
            }
            
            int tamanhoCaminho = 0;
            int no = indiceDestino;
            while (no != -1) {
                caminho[tamanhoCaminho++] = no;
                no = pai[no];
            }
            
            // Reverter caminho
            for (int i = 0; i < tamanhoCaminho / 2; i++) {
                int temp = caminho[i];
                caminho[i] = caminho[tamanhoCaminho - 1 - i];
                caminho[tamanhoCaminho - 1 - i] = temp;
            }
            
            caminho[tamanhoCaminho] = -1; // Marcador de fim
            
            liberarFila(fila);
            free(visitado);
            free(pai);
            return caminho;
        }
        
        // Obter lista de adjacências e ordenar
        Vertice *vertice = &grafo->vertices[atual];
        NoAresta *arestas = vertice->listaAdjacencias;
        
        // Criar lista temporária apenas com arestas ativas
        NoAresta *arestasAtivas = NULL;
        NoAresta *ultimaAtiva = NULL;
        
        NoAresta *temp = arestas;
        while (temp != NULL) {
            if (relacaoAtiva(temp)) {
                // Criar cópia da aresta para a lista de ativas
                NoAresta *copia = criarNoAresta(temp->nomeUsuarioDestino,
                                                 temp->dataInicio,
                                                 temp->dataFim,
                                                 temp->grauAmizade);
                if (copia != NULL) {
                    if (arestasAtivas == NULL) {
                        arestasAtivas = copia;
                        ultimaAtiva = copia;
                    } else {
                        ultimaAtiva->proximo = copia;
                        ultimaAtiva = copia;
                    }
                }
            }
            temp = temp->proximo;
        }
        
        // Ordenar arestas ativas
        arestasAtivas = ordenarListaArestas(arestasAtivas);
        
        // Visitar vizinhos na ordem especificada
        temp = arestasAtivas;
        while (temp != NULL) {
            // Encontrar índice do vértice destino
            int indiceVizinho = buscarIndiceVertice(grafo, temp->nomeUsuarioDestino);
            
            if (indiceVizinho != -1 && !visitado[indiceVizinho]) {
                visitado[indiceVizinho] = 1;
                pai[indiceVizinho] = atual;
                enfileirar(fila, indiceVizinho);
            }
            
            temp = temp->proximo;
        }
        
        // Liberar lista temporária de arestas ativas
        liberarListaAdjacencias(arestasAtivas);
    }
    
    liberarFila(fila);
    free(visitado);
    free(pai);
    return NULL; // Não encontrou caminho
}

// Função principal da funcionalidade 13 (Caminho Mais Curto - Celebridade)
void funcionalidade13(char *arquivoPessoa, char *arquivoIndice, char *arquivoSegue, char *nomeCelebridade) {
    // Construir grafo direto
    Grafo *grafo = construirGrafo(arquivoPessoa, arquivoIndice, arquivoSegue, 0);
    
    if (grafo == NULL) {
        printf("Falha na execução da funcionalidade.\n");
        return;
    }
    
    // Encontrar índice da celebridade
    int indiceCelebridade = buscarIndiceVertice(grafo, nomeCelebridade);
    
    if (indiceCelebridade == -1) {
        // Celebridade não encontrada - erro
        printf("Falha na execução da funcionalidade.\n");
        liberarGrafo(grafo);
        return;
    }
    
    // Para cada vértice, encontrar caminho até a celebridade
    int primeiroCaminho = 1;
    for (int i = 0; i < grafo->quantidadeVertices; i++) {
        if (i == indiceCelebridade) {
            // A celebridade: não precisa imprimir nada (ela não segue a si mesma no contexto do caminho)
            continue;
        }
        
        int *caminho = encontrarCaminhoBFS(grafo, i, indiceCelebridade);
        
        if (caminho == NULL) {
            // Pular linha antes se não for o primeiro caminho
            if (!primeiroCaminho) {
                printf("\n");
            }
            printf("NAO SEGUE A CELEBRIDADE");
            primeiroCaminho = 0;
        } else {
            // Pular linha antes se não for o primeiro caminho
            if (!primeiroCaminho) {
                printf("\n");
            }
            primeiroCaminho = 0;
            
            // Imprimir cada aresta do caminho em uma linha separada
            int j = 0;
            while (caminho[j] != -1 && caminho[j + 1] != -1) {
                Vertice *origem = &grafo->vertices[caminho[j]];
                Vertice *destino = &grafo->vertices[caminho[j + 1]];
                
                // Encontrar aresta entre caminho[j] e caminho[j+1]
                NoAresta *aresta = origem->listaAdjacencias;
                NoAresta *arestaEncontrada = NULL;
                
                while (aresta != NULL) {
                    if (aresta->nomeUsuarioDestino != NULL &&
                        destino->nomeUsuario != NULL &&
                        strcmp(aresta->nomeUsuarioDestino, destino->nomeUsuario) == 0 &&
                        relacaoAtiva(aresta)) {
                        arestaEncontrada = aresta;
                        break;
                    }
                    aresta = aresta->proximo;
                }
                
                if (arestaEncontrada != NULL) {
                    // Formatar dados da aresta
                    char dataInicioFormatada[15];
                    char dataFimFormatada[15];
                    char grauFormatado[10];
                    
                    formatarData(arestaEncontrada->dataInicio, dataInicioFormatada);
                    formatarData(arestaEncontrada->dataFim, dataFimFormatada);
                    formatarGrauAmizade(arestaEncontrada->grauAmizade, grauFormatado);
                    
                    // Imprimir: nomeUsuarioQueSegue, nomeUsuarioQueESeguida, dataInicio, dataFim, grauAmizade
                    printf("%s, %s, %s, %s, %s",
                           origem->nomeUsuario ? origem->nomeUsuario : "NULO",
                           arestaEncontrada->nomeUsuarioDestino ? arestaEncontrada->nomeUsuarioDestino : "NULO",
                           dataInicioFormatada,
                           dataFimFormatada,
                           grauFormatado);
                    
                    // Pular linha após cada aresta
                    printf("\n");
                }
                
                j++;
            }
            free(caminho);
        }
    }
    
    liberarGrafo(grafo);
}

// Função auxiliar para encontrar ciclo usando DFS
// Retorna o comprimento do ciclo encontrado (número de arestas), ou -1 se não encontrou
// profundidade = número de arestas percorridas até agora
static int encontrarCicloDFS(Grafo *grafo, int indiceAtual, int indiceOrigem, 
                              int *visitado, int profundidade, int profundidadeMaxima) {
    // Limite de profundidade para evitar loops infinitos
    if (profundidade >= profundidadeMaxima) {
        return -1;
    }
    
    // Marcar como visitado
    visitado[indiceAtual] = 1;
    
    // Obter lista de adjacências
    Vertice *vertice = &grafo->vertices[indiceAtual];
    NoAresta *arestas = vertice->listaAdjacencias;
    
    // Criar lista temporária com todas as arestas (não apenas ativas)
    // A funcionalidade 14 não especifica filtrar por relações ativas
    NoAresta *arestasOrdenadas = NULL;
    NoAresta *ultima = NULL;
    
    NoAresta *temp = arestas;
    while (temp != NULL) {
        // Criar cópia da aresta para a lista
        NoAresta *copia = criarNoAresta(temp->nomeUsuarioDestino,
                                         temp->dataInicio,
                                         temp->dataFim,
                                         temp->grauAmizade);
        if (copia != NULL) {
            if (arestasOrdenadas == NULL) {
                arestasOrdenadas = copia;
                ultima = copia;
            } else {
                ultima->proximo = copia;
                ultima = copia;
            }
        }
        temp = temp->proximo;
    }
    
    // Ordenar arestas conforme critérios de ordem de visita
    // Ordem: nomeUsuario (alfabético) -> dataInicio -> dataFim
    arestasOrdenadas = ordenarListaArestas(arestasOrdenadas);
    
    // Visitar vizinhos na ordem especificada
    temp = arestasOrdenadas;
    while (temp != NULL) {
        // Encontrar índice do vértice destino
        int indiceVizinho = buscarIndiceVertice(grafo, temp->nomeUsuarioDestino);
        
        if (indiceVizinho != -1) {
            // Se é o vértice de origem e já saímos dele (profundidade > 0), encontramos um ciclo
            // O comprimento do ciclo é profundidade + 1 (arestas percorridas + aresta de retorno)
            if (indiceVizinho == indiceOrigem && profundidade > 0) {
                // Liberar lista temporária
                liberarListaAdjacencias(arestasOrdenadas);
                // Desmarcar antes de retornar
                visitado[indiceAtual] = 0;
                return profundidade + 1; // Comprimento do ciclo (número de arestas)
            }
            
            // Se não foi visitado ainda, continuar a busca
            if (!visitado[indiceVizinho]) {
                int resultado = encontrarCicloDFS(grafo, indiceVizinho, indiceOrigem, 
                                                    visitado, profundidade + 1, profundidadeMaxima);
                if (resultado != -1) {
                    // Encontrou ciclo, liberar e retornar
                    liberarListaAdjacencias(arestasOrdenadas);
                    visitado[indiceAtual] = 0;
                    return resultado;
                }
            }
        }
        
        temp = temp->proximo;
    }
    
    // Liberar lista temporária
    liberarListaAdjacencias(arestasOrdenadas);
    
    // Desmarcar antes de retornar (backtracking)
    visitado[indiceAtual] = 0;
    return -1; // Não encontrou ciclo
}

// Função principal da funcionalidade 14 (Ciclo de Fofoca)
void funcionalidade14(char *arquivoPessoa, char *arquivoIndice, char *arquivoSegue, char *nomeFofoqueiro) {
    // Construir grafo direto
    Grafo *grafo = construirGrafo(arquivoPessoa, arquivoIndice, arquivoSegue, 0);
    
    if (grafo == NULL) {
        printf("Falha na execução da funcionalidade.\n");
        return;
    }
    
    // Encontrar índice do fofoqueiro
    int indiceFofoqueiro = buscarIndiceVertice(grafo, nomeFofoqueiro);
    
    if (indiceFofoqueiro == -1) {
        // Fofoqueiro não encontrado - erro
        printf("Falha na execução da funcionalidade.\n");
        liberarGrafo(grafo);
        return;
    }
    
    // Array para marcar vértices visitados
    int *visitado = calloc(grafo->quantidadeVertices, sizeof(int));
    if (visitado == NULL) {
        printf("Falha na execução da funcionalidade.\n");
        liberarGrafo(grafo);
        return;
    }
    
    // Buscar ciclo começando do fofoqueiro
    // Usar profundidade máxima igual ao número de vértices para evitar loops infinitos
    int comprimentoCiclo = encontrarCicloDFS(grafo, indiceFofoqueiro, indiceFofoqueiro, 
                                               visitado, 0, grafo->quantidadeVertices);
    
    if (comprimentoCiclo == -1) {
        // Não encontrou ciclo
        printf("A FOFOCA NAO RETORNOU\n");
    } else {
        // Encontrou ciclo, imprimir comprimento
        printf("%d\n", comprimentoCiclo);
    }
    
    free(visitado);
    liberarGrafo(grafo);
}

