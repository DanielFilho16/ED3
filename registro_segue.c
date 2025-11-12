#include "registro_segue.h"

// Função auxiliar para escrever um registro segue no arquivo
void escreverRegistroSegue(FILE *arquivo, RegistroSegue *segue) {
    if (arquivo == NULL || segue == NULL) return;
    
    segue->removido = '0';
    fwrite(&segue->removido, sizeof(char), 1, arquivo);
    fwrite(&segue->idPessoaQueSegue, sizeof(int), 1, arquivo);
    fwrite(&segue->idPessoaQueESeguida, sizeof(int), 1, arquivo);
    fwrite(segue->dataInicioQueSegue, sizeof(char), 10, arquivo);
    fwrite(segue->dataFimQueSegue, sizeof(char), 10, arquivo);
    fwrite(&segue->grauAmizade, sizeof(char), 1, arquivo);
}

// Função auxiliar para ler um registro segue do arquivo binário
int lerRegistroSegue(FILE *arquivo, RegistroSegue *segue) {
    if (arquivo == NULL || segue == NULL) return 0;
    
    if (fread(&segue->removido, sizeof(char), 1, arquivo) != 1) {
        return 0; 
    }
    
    if (fread(&segue->idPessoaQueSegue, sizeof(int), 1, arquivo) != 1) return 0;
    if (fread(&segue->idPessoaQueESeguida, sizeof(int), 1, arquivo) != 1) return 0;
    if (fread(segue->dataInicioQueSegue, sizeof(char), 10, arquivo) != 10) {
        return 0;
    }
    
    // Ler dataFimQueSegue
    if (fread(segue->dataFimQueSegue, sizeof(char), 10, arquivo) != 10) {
        return 0;
    }
    
    if (fread(&segue->grauAmizade, sizeof(char), 1, arquivo) != 1) {
        return 0;
    }

    return 1; 
}

// Função de comparação para ordenação dos registros segue
int compararRegistrosSegue(const void *a, const void *b) {
    const RegistroSegue *regA = (const RegistroSegue *)a;
    const RegistroSegue *regB = (const RegistroSegue *)b;

    // Critério 1: idPessoaQueSegue (crescente)
    if (regA->idPessoaQueSegue < regB->idPessoaQueSegue) return -1;
    if (regA->idPessoaQueSegue > regB->idPessoaQueSegue) return 1;

    // Critério 2: idPessoaQueESeguida (crescente) - desempate
    if (regA->idPessoaQueESeguida < regB->idPessoaQueESeguida) return -1;
    if (regA->idPessoaQueESeguida > regB->idPessoaQueESeguida) return 1;

    // Critério 3: dataInicioQueSegue (crescente) - desempate
    // Tratar valores nulos: nulos vêm depois dos não-nulos
    if (regA->dataInicioQueSegue[0] == '$' && regB->dataInicioQueSegue[0] == '$') {
        // Ambos nulos, continuar para próximo critério
    } else if (regA->dataInicioQueSegue[0] == '$') {
        return 1; // A é nulo, B não é nulo, A vem depois
    } else if (regB->dataInicioQueSegue[0] == '$') {
        return -1; // B é nulo, A não é nulo, B vem depois
    } else {
        // Ambos não nulos, comparar strings
        int comparacao = 0;
        int kk = 6;
        while(1) {
            if(regA->dataInicioQueSegue[kk] < regB->dataInicioQueSegue[kk]) comparacao = -1;
            if(regA->dataInicioQueSegue[kk] > regB->dataInicioQueSegue[kk]) comparacao = 1;
            if (comparacao != 0) return comparacao; //se for diferente já retorna
            if(kk == 9) kk = 3; // Pula do ano para o mês
            if(kk == 4) kk = 0; // Pula do mês para o dia
            if(kk == 2) break; // Sai após comparar o dia
            else kk ++;
        }
    }

    // Critério 4: dataFimQueSegue (crescente) - desempate final
    // Tratar valores nulos: nulos vêm depois dos não-nulos
    if (regA->dataFimQueSegue[0] == '$' && regB->dataFimQueSegue[0] == '$') {
        return 0; // Ambos nulos, são iguais
    } else if (regA->dataFimQueSegue[0] == '$') {
        return 1; // A é nulo, B não é nulo, A vem depois
    } else if (regB->dataFimQueSegue[0] == '$') {
        return -1; // B é nulo, A não é nulo, B vem depois
    } else {
        // Ambos não nulos, comparar strings
        int comparacao = 0;
        int kk = 6;
        while(1) {
            if(regA->dataFimQueSegue[kk] < regB->dataFimQueSegue[kk]) comparacao = -1;
            if(regA->dataFimQueSegue[kk] > regB->dataFimQueSegue[kk]) comparacao = 1;
            if (comparacao != 0) return comparacao; //se for diferente já retorna
            if(kk == 9) kk = 4; // Pula do ano para o mês
            if(kk == 5) kk = 0; // Pula do mês para o dia
            if(kk == 2) break; // Sai após comparar o dia
            else kk ++;
        }
    }
    
    return 0;
}

// Funções auxiliares
void inicializarRegistroSegue(RegistroSegue *segue) {
    if (segue == NULL) return;
    segue->removido = '0';
    segue->idPessoaQueSegue = 0;
    segue->idPessoaQueESeguida = 0;
    for (int i = 0; i < 10; i++) {
        segue->dataInicioQueSegue[i] = '$';
        segue->dataFimQueSegue[i] = '$';
    }
    segue->grauAmizade = '$';
}

void imprimirRegistroSegue(RegistroSegue *segue) {
    if (segue == NULL) return;
    printf("Segue a pessoa de codigo: %d\n", segue->idPessoaQueESeguida);
    
    // Justificativa para seguir
    if (segue->grauAmizade == '0') {
        printf("Justificativa para seguir: celebridade\n");
    } else if (segue->grauAmizade == '1') {
        printf("Justificativa para seguir: amiga de minha amiga\n");
    } else if (segue->grauAmizade == '2') {
        printf("Justificativa para seguir: minha amiga\n");
    } else {
        printf("Justificativa para seguir: -\n");
    }
    
    // Imprimir data início
    printf("Começou a seguir em: ");
    int datainvalida = 0;
    for (int j = 0; j < 10; j++) {
        if (segue->dataInicioQueSegue[j] == '$') {
            datainvalida = 1;
            break;
        }
    }
    if (datainvalida) {
        printf("-");
    } else {
        for (int j = 0; j < 10; j++) {
            printf("%c", segue->dataInicioQueSegue[j]);
        }
    }
    printf("\n");
    
    // Imprimir data fim
    printf("Parou de seguir em: ");
    datainvalida = 0;
    for (int j = 0; j < 10; j++) {
        if (segue->dataFimQueSegue[j] == '$') {
            datainvalida = 1;
            break;
        }
    }
    if (datainvalida) {
        printf("-");
    } else {
        for (int j = 0; j < 10; j++) {
            printf("%c", segue->dataFimQueSegue[j]);
        }
    }
    printf("\n\n");
}

// Função para ler todos os registros segue do arquivo para RAM
RegistroSegue** LerRegistrosSegue(char *nomeArquivo, int *tamanho) {
    FILE *arquivo;
    CabecalhoSegue cabecalho;
    RegistroSegue **registros = NULL;
    int capacidade = 1000;
    int quantidadeRegistros = 0;
    
    arquivo = fopen(nomeArquivo, "rb");
    if (arquivo == NULL) {
        return NULL;
    }
    
    // Ler cabeçalho
    if (fread(&cabecalho.status, sizeof(char), 1, arquivo) != 1 ||
        fread(&cabecalho.quantidadeSegue, sizeof(int), 1, arquivo) != 1 ||
        fread(&cabecalho.proxRRN, sizeof(int), 1, arquivo) != 1) {
        fclose(arquivo);
        return NULL;
    }
    
    if (cabecalho.status != '1') {
        fclose(arquivo);
        return NULL;
    }
    
    // Alocar memória para array de ponteiros
    registros = malloc(capacidade * sizeof(RegistroSegue*));
    if (registros == NULL) {
        fclose(arquivo);
        return NULL;
    }
    
    // Ler todos os registros do arquivo
    while (quantidadeRegistros < cabecalho.quantidadeSegue) {
        // Alocar memória para o registro
        registros[quantidadeRegistros] = malloc(sizeof(RegistroSegue));
        if (registros[quantidadeRegistros] == NULL) {
            for (int i = 0; i < quantidadeRegistros; i++) {
                free(registros[i]);
            }
            free(registros);
            fclose(arquivo);
            return NULL;
        }
        
        if (!lerRegistroSegue(arquivo, registros[quantidadeRegistros])) {
            for (int i = 0; i <= quantidadeRegistros; i++) {
                free(registros[i]);
            }
            free(registros);
            fclose(arquivo);
            return NULL;
        }
        
        quantidadeRegistros++;
        
        // Redimensionar array se necessário
        if (quantidadeRegistros >= capacidade) {
            capacidade *= 2;
            registros = realloc(registros, capacidade * sizeof(RegistroSegue*));
            if (registros == NULL) {
                for (int i = 0; i < quantidadeRegistros; i++) {
                    free(registros[i]);
                }
                free(registros);
                fclose(arquivo);
                return NULL;
            }
        }
    }
    
    fclose(arquivo);
    *tamanho = quantidadeRegistros;
    return registros;
}

// Função para liberar memória dos registros segue
void LiberarRegistrosSegue(RegistroSegue **registros, int tamanho) {
    if (registros == NULL) return;
    for (int i = 0; i < tamanho; i++) {
        if (registros[i] != NULL) {
            free(registros[i]);
        }
    }
    free(registros);
}

