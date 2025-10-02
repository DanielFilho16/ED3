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

// Implementações vazias das outras funções (serão implementadas posteriormente)
void CREAT_TABLE(char *csvArquivo, char *binArquivo, char *indiceArquivo) {
    printf("Funcionalidade ainda não implementada.\n");
}

void SELECT(char *binArquivo) {
    printf("Funcionalidade ainda não implementada.\n");
}

void SELECT_WHERE(char *binArquivo, char *indiceArquivo) {
    printf("Funcionalidade ainda não implementada.\n");
}
