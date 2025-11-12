#include "registro_pessoa.h"

#define debug 0

// calcula o tamanho do registro
int calcularTamanhoRegistro(RegistroPessoa *pessoa) {
    if (pessoa == NULL) return 0;
    return 4 + 4 + 4 + pessoa->tamanhoNomePessoa + 4 + pessoa->tamanhoNomeUsuario;
}

// escreve um registro pessoa no arquivo
void escreverRegistroPessoa(FILE *arquivo, RegistroPessoa *pessoa) {
    if (arquivo == NULL || pessoa == NULL) return;
    
    pessoa->removido = '0';
    if(pessoa->tamanhoRegistro == 0) pessoa->tamanhoRegistro = calcularTamanhoRegistro(pessoa);

    fwrite(&pessoa->removido, sizeof(char), 1, arquivo);
    fwrite(&pessoa->tamanhoRegistro, sizeof(int), 1, arquivo);
    fwrite(&pessoa->idPessoa, sizeof(int), 1, arquivo);
    fwrite(&pessoa->idadePessoa, sizeof(int), 1, arquivo);
    fwrite(&pessoa->tamanhoNomePessoa, sizeof(int), 1, arquivo);

    if (pessoa->tamanhoNomePessoa > 0) {
        fwrite(pessoa->nomePessoa, sizeof(char), pessoa->tamanhoNomePessoa, arquivo);
    }

    fwrite(&pessoa->tamanhoNomeUsuario, sizeof(int), 1, arquivo);

    if (pessoa->tamanhoNomeUsuario > 0) {
        fwrite(pessoa->nomeUsuario, sizeof(char), pessoa->tamanhoNomeUsuario, arquivo);
    }
}

int lerRegistroPessoa(FILE *arquivo, RegistroPessoa *pessoa) {
    if (arquivo == NULL || pessoa == NULL) return 0;
    
    int byteatual = ftell(arquivo);
    if(debug) fprintf(stderr,"Lendo registro no byte offset: %d\n",byteatual);
    if (fread(&pessoa->removido, sizeof(char), 1, arquivo) != 1) {
        return 0; 
    }
    if(debug) fprintf(stderr,"Lendo registro com status: %c\n",pessoa->removido);
    if (fread(&pessoa->tamanhoRegistro, sizeof(int), 1, arquivo) != 1) {
        return 0;
    }
    if(debug) fprintf(stderr,"Tamanho do registro: %d\n",pessoa->tamanhoRegistro);
    if (fread(&pessoa->idPessoa, sizeof(int), 1, arquivo) != 1) return 0;
    if(debug) fprintf(stderr,"ID da pessoa: %d\n",pessoa->idPessoa);
    if (fread(&pessoa->idadePessoa, sizeof(int), 1, arquivo) != 1) return 0;
    if(debug) fprintf(stderr,"Idade da pessoa: %d\n",pessoa->idadePessoa);
    if (fread(&pessoa->tamanhoNomePessoa, sizeof(int), 1, arquivo) != 1) return 0;
    if(debug) fprintf(stderr,"Tamanho do nome da pessoa: %d\n",pessoa->tamanhoNomePessoa);

    if (pessoa->tamanhoNomePessoa > 0) {
        // Validar tamanhoNomePessoa antes de alocar
        if (pessoa->tamanhoNomePessoa <= 0) {
            fprintf(stderr, "Erro: tamanhoNomePessoa inválido (%d)\n", pessoa->tamanhoNomePessoa);
            return 0;
        }

        pessoa->nomePessoa = malloc(pessoa->tamanhoNomePessoa + 1);
        if (pessoa->nomePessoa == NULL) { 
            fprintf(stderr, "Erro: Falha ao alocar memória para nomePessoa (l379).\n");
            fprintf(stderr,"ERRO: Falha ao alocar %d bits de memória para ID %d",pessoa->tamanhoNomePessoa,pessoa->idPessoa);
            return 0;
        } 
        if (fread(pessoa->nomePessoa, sizeof(char), pessoa->tamanhoNomePessoa, arquivo) != pessoa->tamanhoNomePessoa) {
            free(pessoa->nomePessoa);
            pessoa->nomePessoa = NULL;
            return 0;
        }
        pessoa->nomePessoa[pessoa->tamanhoNomePessoa] = '\0';
    } else {
        pessoa->nomePessoa = NULL;
    }

    // Ler nomeUsuario
    if (fread(&pessoa->tamanhoNomeUsuario, sizeof(int), 1, arquivo) != 1) {
        if (pessoa->nomePessoa) free(pessoa->nomePessoa);
        return 0;
    }

    if (pessoa->tamanhoNomeUsuario > 0) {
        pessoa->nomeUsuario = malloc(pessoa->tamanhoNomeUsuario + 1);
        if (pessoa->nomeUsuario == NULL) { 
            fprintf(stderr, "Erro: Falha ao alocar memória para nomePessoa. (l400)\n ");
            printf("ERRO: Falha ao alocar memória para ID %d",pessoa->idPessoa);
            return 0;
        } 

        if (fread(pessoa->nomeUsuario, sizeof(char), pessoa->tamanhoNomeUsuario, arquivo) != pessoa->tamanhoNomeUsuario) {
            if (pessoa->nomePessoa) free(pessoa->nomePessoa);
            free(pessoa->nomeUsuario);
            return 0;
        }
        pessoa->nomeUsuario[pessoa->tamanhoNomeUsuario] = '\0';
    } else {
        pessoa->nomeUsuario = NULL;
    }
    fseek(arquivo, byteatual + 5 + pessoa->tamanhoRegistro, SEEK_SET);
    return 1; 
}

// Funções auxiliares de registro
void inicializarRegistroPessoa(RegistroPessoa *pessoa) {
    if (pessoa == NULL) return;
    pessoa->removido = '0';
    pessoa->tamanhoRegistro = 0;
    pessoa->idPessoa = 0;
    pessoa->idadePessoa = -1;
    pessoa->tamanhoNomePessoa = 0;
    pessoa->nomePessoa = NULL;
    pessoa->tamanhoNomeUsuario = 0;
    pessoa->nomeUsuario = NULL;
}

void liberarRegistroPessoa(RegistroPessoa *pessoa) {
    if (pessoa == NULL) return;
    if (pessoa->nomePessoa) {
        free(pessoa->nomePessoa);
        pessoa->nomePessoa = NULL;
    }
    if (pessoa->nomeUsuario) {
        free(pessoa->nomeUsuario);
        pessoa->nomeUsuario = NULL;
    }
}

int copiarRegistroPessoa(RegistroPessoa *destino, RegistroPessoa *origem) {
    if (destino == NULL || origem == NULL) return 0;
    
    destino->removido = origem->removido;
    destino->tamanhoRegistro = origem->tamanhoRegistro;
    destino->idPessoa = origem->idPessoa;
    destino->idadePessoa = origem->idadePessoa;
    destino->tamanhoNomePessoa = origem->tamanhoNomePessoa;
    destino->tamanhoNomeUsuario = origem->tamanhoNomeUsuario;
    
    destino->nomePessoa = NULL;
    destino->nomeUsuario = NULL;
    
    if (origem->nomePessoa) {
        destino->nomePessoa = malloc(origem->tamanhoNomePessoa + 1);
        if (destino->nomePessoa == NULL) return 0;
        strcpy(destino->nomePessoa, origem->nomePessoa);
    }
    
    if (origem->nomeUsuario) {
        destino->nomeUsuario = malloc(origem->tamanhoNomeUsuario + 1);
        if (destino->nomeUsuario == NULL) {
            if (destino->nomePessoa) free(destino->nomePessoa);
            return 0;
        }
        strcpy(destino->nomeUsuario, origem->nomeUsuario);
    }
    
    return 1;
}

// Funções de impressão
void imprimirRegistroPessoa(RegistroPessoa *pessoa) {
    if (pessoa == NULL) return;
    
    printf("Dados da pessoa de codigo %d\n", pessoa->idPessoa);
    printf("Nome: %s\n", pessoa->nomePessoa ? pessoa->nomePessoa : "-");
    if (pessoa->idadePessoa == -1) {
        printf("Idade: -\n");
    } else {
        printf("Idade: %d\n", pessoa->idadePessoa);
    }
    printf("Usuario: %s\n", pessoa->nomeUsuario ? pessoa->nomeUsuario : "-");
    printf("\n");
}

// Funções de manipulação em arquivo
int marcarRegistroRemovido(FILE *arquivo, long long posicao) {
    if (arquivo == NULL) return 0;
    fseek(arquivo, posicao, SEEK_SET);
    char removido = '1';
    if (fwrite(&removido, sizeof(char), 1, arquivo) != 1) return 0;
    return 1;
}

int lerRegistroEmPosicao(FILE *arquivo, long long posicao, RegistroPessoa *pessoa) {
    if (arquivo == NULL || pessoa == NULL) return 0;
    fseek(arquivo, posicao, SEEK_SET);
    return lerRegistroPessoa(arquivo, pessoa);
}

void escreverRegistroEmPosicao(FILE *arquivo, long long posicao, RegistroPessoa *pessoa) {
    if (arquivo == NULL || pessoa == NULL) return;
    fseek(arquivo, posicao, SEEK_SET);
    escreverRegistroPessoa(arquivo, pessoa);
}

void preencherComLixo(FILE *arquivo, long long posicao, int tamanho) {
    if (arquivo == NULL) return;
    fseek(arquivo, posicao, SEEK_SET);
    for (int i = 0; i < tamanho; i++) {
        char lixo = '$';
        fwrite(&lixo, sizeof(char), 1, arquivo);
    }
}

