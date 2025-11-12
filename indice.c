#include "indice.h"
#include "io_arquivos.h"
#include "utils.h"

#define debug 0
#define debugi 0

// ordena idPessoa para o indice
int compararIdPessoa(const void *a, const void *b) {
    const RegistroIndice *regA = (const RegistroIndice *)a;
    const RegistroIndice *regB = (const RegistroIndice *)b;

    if (regA->idPessoa < regB->idPessoa) return -1;
    if (regA->idPessoa > regB->idPessoa) return 1;
    return 0;
}

// função 01 - Criar índice
void criarIndice(char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "wb");

    if (arquivo == NULL) { 
        printf("Falha no processamento do arquivo.\n");
        exit (0);
    }

    CabecalhoIndice cabecalho;
    inicializarCabecalhoIndice(&cabecalho);

    fwrite(&cabecalho.status, 1, 1, arquivo);
    fwrite(&cabecalho.lixo, 11, 1, arquivo);

    // fechar arquivo e marcar status como fechado
    cabecalho.status = '1';
    fseek(arquivo, 0, SEEK_SET);
    fwrite(&cabecalho.status, sizeof(char), 1, arquivo);

    fclose(arquivo);

    // Mostrar arquivo na tela usando binarioNaTela
    binarioNaTela(nomeArquivo);
}

// Função para carregar índice em memória
RegistroIndice* CarregarIndiceEmMemoria(char *nomeArquivo, int *tamanho) {
    if(debug) printf("carregando indice em memoria...\n");
    
    FILE *arquivo;
    CabecalhoIndice cabecalho;
    RegistroIndice *registros = NULL;
    int capacidade = 1000;
    int quantidadeRegistros = 0;

    arquivo = fopen(nomeArquivo, "rb");

    if (arquivo == NULL) {
        printf("Falha no processamento do arquivo.\n");
        exit(0);
    }
    
    // Ler cabeçalho
    if (!lerCabecalhoIndice(arquivo, &cabecalho)) {
        fclose(arquivo);
        if(debug) printf("erro ao ler o cabeçalho\n");
        return NULL;
    }
    
    if (!validarArquivoConsistente(cabecalho.status)) {
        fclose(arquivo);
        if(debug) printf("arquivo inconsistente\n");
        return NULL;
    }
    
    // Calcular quantidade de registros no índice
    fseek(arquivo, 0, SEEK_END);
    long long tamanhoArquivo = ftell(arquivo);
    int totalRegistros = (tamanhoArquivo - 12) / 12; // 12 bytes por registro (4 + 8)
    
    if (totalRegistros <= 0) {
        fclose(arquivo);
        *tamanho = 0;
        if(debug) printf("nenhum registro no indice\n");
        return NULL;
    }
    
    // Alocar memória para os registros
    registros = malloc(totalRegistros * sizeof(RegistroIndice));
    if (registros == NULL) {
        fclose(arquivo);
        if(debug) printf("falha ao alocar memoria para os registros\n");
        return NULL;
    }
    
    // Ler todos os registros do índice
    fseek(arquivo, 12, SEEK_SET);
    for (int i = 0; i < totalRegistros; i++) {
        if (fread(&registros[i].idPessoa, sizeof(int), 1, arquivo) != 1 ||
            fread(&registros[i].byteOffset, sizeof(long long), 1, arquivo) != 1) {
            free(registros);
            fclose(arquivo);
            if(debug) printf("erro ao ler os registros do indice\n");
            return NULL;
        }
        quantidadeRegistros++;
    }
    
    fclose(arquivo);
    *tamanho = quantidadeRegistros;
    if(debug) printf("indice carregado em memoria com %d registros\n", quantidadeRegistros);
    return registros;
}

// Função para remover registro do índice em memória
void RemoverDoIndice(RegistroIndice **registros, int *tamanho, int idPessoa) {
    if (registros == NULL || *registros == NULL) return;
    
    for (int i = 0; i < *tamanho; i++) {
        if ((*registros)[i].idPessoa == idPessoa) {
            // Mover todos os registros uma posição para a esquerda
            for (int j = i; j < *tamanho - 1; j++) {
                (*registros)[j] = (*registros)[j + 1];
            }
            (*tamanho)--;
            break;
        }
    }
}

// Função para reescrever arquivo de índice
void ReescreverIndice(char *nomeArquivo, RegistroIndice *registros, int tamanho) {
    FILE *arquivo;
    CabecalhoIndice cabecalho;
    
    // Abrir arquivo no modo escrita binária (apaga conteúdo)
    arquivo = fopen(nomeArquivo, "wb");
    if (arquivo == NULL) {
        return;
    }
    
    // Escrever cabeçalho
    inicializarCabecalhoIndice(&cabecalho);
    escreverCabecalhoIndice(arquivo, &cabecalho);
    
    // Escrever todos os registros
    fseek(arquivo, 12, SEEK_SET);
    for (int i = 0; i < tamanho; i++) {
        fwrite(&registros[i].idPessoa, sizeof(int), 1, arquivo);
        fwrite(&registros[i].byteOffset, sizeof(long long), 1, arquivo);
    }
    
    // Marcar arquivo como consistente
    cabecalho.status = '1';
    escreverCabecalhoIndice(arquivo, &cabecalho);
    
    fclose(arquivo);
}

// Função auxiliar para atualizar índice
void atualizarIndice(char *indiceArquivo, int idPessoa, long long byteOffset) {
    FILE *arquivo;
    CabecalhoIndice cabecalho;
    RegistroIndice novoRegistro;
    RegistroIndice *registros;
    int quantidadeRegistros;
    int i;
    int idJaExiste = 0;
    int posicaoExistente = -1;
    int posicaoInsercao;

    arquivo = fopen(indiceArquivo, "r+b");
    if (arquivo == NULL) {
        printf("Falha no processamento do arquivo.\n");
        exit(0);
    }

    // Ler cabeçalho
    if (!lerCabecalhoIndice(arquivo, &cabecalho)) {
        printf("Falha no processamento do arquivo.\n");
        fclose(arquivo);
        exit(0);
    }

    if(debugi) fprintf(stderr,"Cabeçalho do índice lido: status= %c\n", cabecalho.status);

    if (!validarArquivoConsistente(cabecalho.status)) {
        printf("Falha no processamento do arquivo.\n");
        fclose(arquivo);
        exit(0);
    }

    // Calcular quantidade de registros no índice
    fseek(arquivo, 0, SEEK_END);
    long long tamanhoArquivo = ftell(arquivo);
    quantidadeRegistros = (tamanhoArquivo - 12) / 12; // 12 bytes por registro (4 + 8)

    // Ler todos os registros do índice
    registros = malloc((quantidadeRegistros + 1) * sizeof(RegistroIndice));
    if (registros == NULL) {
        printf("Falha no processamento do arquivo.\n");
        fclose(arquivo);
        exit(0);
    }

    if(debugi) fprintf(stderr,"Lendo %d registros do índice para atualização...\n", quantidadeRegistros);

    fseek(arquivo, 12, SEEK_SET);
    for (i = 0; i < quantidadeRegistros; i++) {
        fread(&registros[i].idPessoa, sizeof(int), 1, arquivo);
        fread(&registros[i].byteOffset, sizeof(long long), 1, arquivo);

        //verifica se o ID já existe
        if (registros[i].idPessoa == idPessoa) {
            idJaExiste = 1;
            posicaoExistente = i;
            if(debugi) fprintf(stderr,"ID %d já existe no índice na posição %d\n", 
                              idPessoa, posicaoExistente);
        }
    }

    // Adicionar novo registro
    novoRegistro.idPessoa = idPessoa;
    novoRegistro.byteOffset = byteOffset;

    // Encontrar posição correta para inserir (ordem crescente por ID)
    if(!idJaExiste){
        posicaoInsercao = quantidadeRegistros;
        for (i = 0; i < quantidadeRegistros; i++) {
            if (registros[i].idPessoa > idPessoa) {
                posicaoInsercao = i;
                break;
            }
        }
        if(debugi) fprintf(stderr,"Posição de inserção para ID %d: %d\n", idPessoa, posicaoInsercao);
        // Deslocar registros para abrir espaço
        for (i = quantidadeRegistros; i > posicaoInsercao; i--) {
            registros[i] = registros[i-1];
        }
        if(debugi) fprintf(stderr,"Deslocados registros para inserção do novo registro.\n");
        // Inserir novo registro
        registros[posicaoInsercao] = novoRegistro;
        quantidadeRegistros++;
    } else {
        // Atualizar byteOffset do registro existente
        registros[posicaoExistente].byteOffset = byteOffset;
        if(debugi) fprintf(stderr,"Atualizado byteOffset do ID %d na posição %d\n", idPessoa, posicaoExistente);
    }
    // Marcar arquivo como inconsistente
    marcarArquivoInconsistente(arquivo, 1);

    // Reescrever todos os registros
    fseek(arquivo, 12, SEEK_SET);
    for (i = 0; i < quantidadeRegistros; i++) {
        fwrite(&registros[i].idPessoa, sizeof(int), 1, arquivo);
        fwrite(&registros[i].byteOffset, sizeof(long long), 1, arquivo);
    }

    // Marcar arquivo como consistente
    marcarArquivoConsistente(arquivo, 1);

    if(debugi)fprintf(stderr,"Indice atualizado com ID %d no byte offset %lld\n", idPessoa, byteOffset);

    free(registros);
    fclose(arquivo);
}

