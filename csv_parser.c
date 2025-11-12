#include "csv_parser.h"

// função para ler csv
int ler_csv(FILE *csv, RegistroPessoa *pessoa) {
    char linha[1024];
    char *campos[4];
    int numCampos = 0;

    if (fgets(linha, sizeof(linha), csv) == NULL) {
        return 0; 
    }

    linha[strcspn(linha, "\r\n")] = '\0';

    if (strlen(linha) == 0) {
        return 0; // Linha vazia, fim do processamento
    }

    char *ptr = linha;
    char *inicio = ptr;

    for (int i = 0; i < 4; i++) {
        inicio = ptr;

        while (*ptr && *ptr != ',') {
            ptr++;
        }

        int tamanho = ptr - inicio;
        campos[i] = malloc(tamanho + 1);
        strncpy(campos[i], inicio, tamanho);
        campos[i][tamanho] = '\0';

        numCampos++;

        if (*ptr == ',') {
            ptr++;
        } else {
            for (int j = i + 1; j < 4; j++) {
                campos[j] = malloc(1);
                campos[j][0] = '\0';
                numCampos++;
            }
            break;
        }
    }

    if (numCampos == 0 || strlen(campos[0]) == 0) {
        for (int i = 0; i < numCampos; i++) {
            free(campos[i]);
        }
        return 0;
    }

    pessoa->idPessoa = atoi(campos[0]);

    if (numCampos > 1 && strlen(campos[1]) > 0) {
        pessoa->tamanhoNomePessoa = strlen(campos[1]);
        pessoa->nomePessoa = malloc(pessoa->tamanhoNomePessoa + 1);
        strcpy(pessoa->nomePessoa, campos[1]);
    } else {
        pessoa->tamanhoNomePessoa = 0;
        pessoa->nomePessoa = NULL;
    }

    if (numCampos > 2 && strlen(campos[2]) > 0) {
        pessoa->idadePessoa = atoi(campos[2]);
    } else {
        pessoa->idadePessoa = -1; // Valor nulo
    }

    if (numCampos > 3 && strlen(campos[3]) > 0) {
        pessoa->tamanhoNomeUsuario = strlen(campos[3]);
        pessoa->nomeUsuario = malloc(pessoa->tamanhoNomeUsuario + 1);
        strcpy(pessoa->nomeUsuario, campos[3]);
    } else {
        pessoa->tamanhoNomeUsuario = 0;
        pessoa->nomeUsuario = NULL;
    }

    for (int i = 0; i < numCampos; i++) {
        free(campos[i]);
    }

    return 1; 
}

// Função para ler CSV do arquivo segue
int ler_csv_segue(FILE *csv, RegistroSegue *segue) {
    char linha[1024];
    char *campos[6];
    int numCampos = 0;

    if (fgets(linha, sizeof(linha), csv) == NULL) {
        return 0; 
    }

    linha[strcspn(linha, "\r\n")] = '\0';

    if (strlen(linha) == 0) {
        return 0; // Linha vazia, fim do processamento
    }

    char *ptr = linha;
    char *inicio = ptr;

    for (int i = 0; i < 6; i++) {
        inicio = ptr;

        while (*ptr && *ptr != ',') {
            ptr++;
        }

        int tamanho = ptr - inicio;
        campos[i] = malloc(tamanho + 1);
        strncpy(campos[i], inicio, tamanho);
        campos[i][tamanho] = '\0';

        numCampos++;

        if (*ptr == ',') {
            ptr++;
        } else {
            for (int j = i + 1; j < 6; j++) {
                campos[j] = malloc(1);
                campos[j][0] = '\0';
                numCampos++;
            }
            break;
        }
    }

    if (numCampos == 0 || strlen(campos[0]) == 0) {
        for (int i = 0; i < numCampos; i++) {
            free(campos[i]);
        }
        return 0;
    }

    segue->idPessoaQueSegue = atoi(campos[0]);
    segue->idPessoaQueESeguida = atoi(campos[1]);

    if (numCampos > 2 && strlen(campos[2]) == 10) {
        strcpy(segue->dataInicioQueSegue, campos[2]);
    } else {
        for(int kk = 0; kk < 10; kk++) {
            segue->dataInicioQueSegue[kk] = '$';
        }
    }

    if (numCampos > 3 && strlen(campos[3]) == 10) {
        strcpy(segue->dataFimQueSegue, campos[3]);
    } else {
        for(int kk = 0; kk < 10; kk++) {
            segue->dataFimQueSegue[kk] = '$';
        }
    }

    if (numCampos > 4 && strlen(campos[4]) > 0) {
        segue->grauAmizade = campos[4][0];
    } else {
        segue->grauAmizade = '$'; // Valor nulo
    }

    for (int i = 0; i < numCampos; i++) {
        free(campos[i]);
    }

    return 1; 
}

// Função auxiliar para pular cabeçalho CSV
void pularCabecalhoCSV(FILE *csv) {
    char linha[1024];
    if (csv != NULL) {
        fgets(linha, sizeof(linha), csv);
    }
}

