#include "estruturas.h"

// Função para criar um arquivo de índice primário vazio
void CREAT_INDEX(char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "wb");
    
    if (arquivo == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }
    
    // Criar e inicializar cabeçalho do índice
    CabecalhoIndice cabecalho;
    cabecalho.status = '0';  // Status aberto
    
    // Preencher lixo com '$'
    for (int i = 0; i < 11; i++) {
        cabecalho.lixo[i] = '$';
    }
    
    // Escrever cabeçalho no arquivo
    fwrite(&cabecalho, sizeof(CabecalhoIndice), 1, arquivo);
    
    // Fechar arquivo e marcar status como fechado
    cabecalho.status = '1';
    fseek(arquivo, 0, SEEK_SET);
    fwrite(&cabecalho.status, sizeof(char), 1, arquivo);
    
    fclose(arquivo);
    
    // Mostrar arquivo na tela usando binarioNaTela
    binarioNaTela(nomeArquivo);
}

// Função para mostrar conteúdo binário na tela (fornecida pelo professor)
void binarioNaTela(char *nomeArquivoSaida) {
    /* Use essa função para comparação no run.codes. Lembre-se de ter fechado (fclose) o arquivo anteriormente.
    *  Ela vai abrir de novo para leitura e depois fechar (você não vai perder pontos por isso se usar ela). */

    unsigned long i, cs;
    unsigned char *mb;
    size_t fl;
    FILE *fs;
    if(nomeArquivoSaida == NULL || !(fs = fopen(nomeArquivoSaida, "rb"))) {
        fprintf(stderr, "ERRO AO ESCREVER O BINARIO NA TELA (função binarioNaTela): não foi possível abrir o arquivo que me passou para leitura. Ele existe e você tá passando o nome certo? Você lembrou de fechar ele com fclose depois de usar?\n");
        return;
    }
    fseek(fs, 0, SEEK_END);
    fl = ftell(fs);
    fseek(fs, 0, SEEK_SET);
    mb = (unsigned char *) malloc(fl);
    fread(mb, 1, fl, fs);

    cs = 0;
    for(i = 0; i < fl; i++) {
        cs += (unsigned long) mb[i];
    }
    printf("%lX\n", cs);
    free(mb);
    fclose(fs);
}

// Função auxiliar para ler uma linha do CSV e parsear os dados
int lerLinhaCSV(FILE *csv, RegistroPessoa *pessoa) {
    char linha[1024];
    char *token;
    
    if (fgets(linha, sizeof(linha), csv) == NULL) {
        return 0; // Fim do arquivo
    }
    
    // Remove quebra de linha
    linha[strcspn(linha, "\n")] = '\0';
    
    // Ignora linhas vazias
    if (strlen(linha) == 0) {
        return 0;
    }
    
    // Parse do idPessoa (primeira coluna)
    token = strtok(linha, ",");
    if (token == NULL) return 0;
    pessoa->idPessoa = atoi(token);
    
    // Parse do nomePessoa (segunda coluna)
    token = strtok(NULL, ",");
    if (token == NULL || strlen(token) == 0) {
        pessoa->tamanhoNomePessoa = 0;
        pessoa->nomePessoa = NULL;
    } else {
        pessoa->tamanhoNomePessoa = strlen(token);
        pessoa->nomePessoa = malloc(pessoa->tamanhoNomePessoa + 1);
        strcpy(pessoa->nomePessoa, token);
    }
    
    // Parse da idade (terceira coluna)
    token = strtok(NULL, ",");
    if (token == NULL || strlen(token) == 0) {
        pessoa->idadePessoa = -1; // Valor nulo
    } else {
        pessoa->idadePessoa = atoi(token);
    }
    
    // Parse do nomeUsuario (quarta coluna)
    token = strtok(NULL, ",");
    if (token == NULL || strlen(token) == 0) {
        pessoa->tamanhoNomeUsuario = 0;
        pessoa->nomeUsuario = NULL;
    } else {
        pessoa->tamanhoNomeUsuario = strlen(token);
        pessoa->nomeUsuario = malloc(pessoa->tamanhoNomeUsuario + 1);
        strcpy(pessoa->nomeUsuario, token);
    }
    
    return 1; // Sucesso
}

// Função auxiliar para calcular o tamanho do registro
int calcularTamanhoRegistro(RegistroPessoa *pessoa) {
    return 1 + 4 + 4 + 4 + 4 + pessoa->tamanhoNomePessoa + 4 + pessoa->tamanhoNomeUsuario;
}

// Função auxiliar para escrever um registro pessoa no arquivo
void escreverRegistroPessoa(FILE *arquivo, RegistroPessoa *pessoa) {
    pessoa->removido = '0'; // Não removido
    pessoa->tamanhoRegistro = calcularTamanhoRegistro(pessoa);
    
    // Escrever campos fixos
    fwrite(&pessoa->removido, sizeof(char), 1, arquivo);
    fwrite(&pessoa->tamanhoRegistro, sizeof(int), 1, arquivo);
    fwrite(&pessoa->idPessoa, sizeof(int), 1, arquivo);
    fwrite(&pessoa->idadePessoa, sizeof(int), 1, arquivo);
    
    // Escrever nomePessoa
    fwrite(&pessoa->tamanhoNomePessoa, sizeof(int), 1, arquivo);
    if (pessoa->tamanhoNomePessoa > 0) {
        fwrite(pessoa->nomePessoa, sizeof(char), pessoa->tamanhoNomePessoa, arquivo);
    }
    
    // Escrever nomeUsuario
    fwrite(&pessoa->tamanhoNomeUsuario, sizeof(int), 1, arquivo);
    if (pessoa->tamanhoNomeUsuario > 0) {
        fwrite(pessoa->nomeUsuario, sizeof(char), pessoa->tamanhoNomeUsuario, arquivo);
    }
}

// Funcionalidade 2: Criar tabela a partir de CSV
void CREAT_TABLE(char *csvArquivo, char *binArquivo, char *indiceArquivo) {
    FILE *csv, *binFile, *indiceFile;
    CabecalhoPessoa cabecalhoPessoa;
    CabecalhoIndice cabecalhoIndice;
    RegistroPessoa pessoa;
    RegistroIndice registroIndice;
    int quantidadePessoas = 0;
    long long offsetAtual;
    
    // Abrir arquivo CSV
    csv = fopen(csvArquivo, "r");
    if (csv == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }
    
    // Abrir arquivo binário pessoa
    binFile = fopen(binArquivo, "wb");
    if (binFile == NULL) {
        printf("Falha no processamento do arquivo.\n");
        fclose(csv);
        return;
    }
    
    // Abrir arquivo de índice
    indiceFile = fopen(indiceArquivo, "r+b");
    if (indiceFile == NULL) {
        printf("Falha no processamento do arquivo.\n");
        fclose(csv);
        fclose(binFile);
        return;
    }
    
    // Inicializar cabeçalho do arquivo pessoa
    cabecalhoPessoa.status = '0';
    cabecalhoPessoa.quantidadePessoas = 0;
    cabecalhoPessoa.quantidadeRemovidos = 0;
    cabecalhoPessoa.proxByteOffset = 17; // Tamanho do cabeçalho
    
    // Escrever cabeçalho inicial do arquivo pessoa
    fwrite(&cabecalhoPessoa, sizeof(CabecalhoPessoa), 1, binFile);
    
    // Atualizar status do arquivo de índice para aberto
    cabecalhoIndice.status = '0';
    fwrite(&cabecalhoIndice.status, sizeof(char), 1, indiceFile);
    
    // Pular a primeira linha (cabeçalho) do CSV
    char linha[1024];
    fgets(linha, sizeof(linha), csv);
    
    // Ler e processar cada linha do CSV
    while (lerLinhaCSV(csv, &pessoa)) {
        // Salvar offset atual para o índice
        offsetAtual = ftell(binFile);
        
        // Escrever registro no arquivo pessoa
        escreverRegistroPessoa(binFile, &pessoa);
        
        // Adicionar entrada no índice
        registroIndice.idPessoa = pessoa.idPessoa;
        registroIndice.byteOffset = offsetAtual;
        fseek(indiceFile, 0, SEEK_END);
        fwrite(&registroIndice, sizeof(RegistroIndice), 1, indiceFile);
        
        quantidadePessoas++;
        
        // Liberar memória alocada
        if (pessoa.nomePessoa) free(pessoa.nomePessoa);
        if (pessoa.nomeUsuario) free(pessoa.nomeUsuario);
    }
    
    // Atualizar cabeçalho do arquivo pessoa
    cabecalhoPessoa.status = '1';
    cabecalhoPessoa.quantidadePessoas = quantidadePessoas;
    cabecalhoPessoa.proxByteOffset = ftell(binFile);
    
    fseek(binFile, 0, SEEK_SET);
    fwrite(&cabecalhoPessoa, sizeof(CabecalhoPessoa), 1, binFile);
    
    // Atualizar status do arquivo de índice para fechado
    cabecalhoIndice.status = '1';
    fseek(indiceFile, 0, SEEK_SET);
    fwrite(&cabecalhoIndice.status, sizeof(char), 1, indiceFile);
    
    // Fechar arquivos
    fclose(csv);
    fclose(binFile);
    fclose(indiceFile);
    
    // Mostrar conteúdo dos arquivos
    binarioNaTela(binArquivo);
    binarioNaTela(indiceArquivo);
}

void SELECT(char *binArquivo) {
    printf("Funcionalidade ainda não implementada.\n");
}

void SELECT_WHERE(char *binArquivo, char *indiceArquivo) {
    printf("Funcionalidade ainda não implementada.\n");
}
