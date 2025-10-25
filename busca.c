#include "busca.h"

// Função auxiliar para verificar se um registro atende ao critério de busca
int verificarMatch(RegistroPessoa *pessoa, char *nomeCampo, char *valorBusca) {
    if (strcmp(nomeCampo, "idPessoa") == 0) {
        int idProcurado = atoi(valorBusca);
        return (pessoa->idPessoa == idProcurado);
    } else if (strcmp(nomeCampo, "nomePessoa") == 0) {
        if (strcmp(valorBusca, "NULO") == 0) {
            return (pessoa->nomePessoa == NULL || strlen(pessoa->nomePessoa) == 0);
        } else if (pessoa->nomePessoa && strcmp(pessoa->nomePessoa, valorBusca) == 0) {
            return 1;
        } else if (!pessoa->nomePessoa && strlen(valorBusca) == 0) {
            return 1;
        }
    } else if (strcmp(nomeCampo, "idade") == 0 || strcmp(nomeCampo, "idadePessoa") == 0) {
        if (strcmp(valorBusca, "NULO") == 0) {
            return (pessoa->idadePessoa == -1);
        } else {
            int idadeProcurada = atoi(valorBusca);
            return (pessoa->idadePessoa == idadeProcurada);
        }
    } else if (strcmp(nomeCampo, "nomeUsuario") == 0) {
        if (strcmp(valorBusca, "NULO") == 0) {
            return (pessoa->nomeUsuario == NULL || strlen(pessoa->nomeUsuario) == 0);
        } else if (pessoa->nomeUsuario && strcmp(pessoa->nomeUsuario, valorBusca) == 0) {
            return 1;
        } else if (!pessoa->nomeUsuario && strlen(valorBusca) == 0) {
            return 1;
        }
    }
    return 0;
}

// Função principal de busca modularizada
ResultadoBusca* buscarPessoas(char *binArquivo, char *indiceArquivo, char *nomeCampo, char *valorBusca) {
    ResultadoBusca *resultado = malloc(sizeof(ResultadoBusca));
    if (resultado == NULL) {
        return NULL;
    }
    
    resultado->pessoas = NULL;
    resultado->quantidade = 0;
    resultado->posicoes = NULL;
    
    int capacidade = 100;
    resultado->pessoas = malloc(capacidade * sizeof(RegistroPessoa));
    resultado->posicoes = malloc(capacidade * sizeof(long long));
    
    if (resultado->pessoas == NULL || resultado->posicoes == NULL) {
        liberarResultadoBusca(resultado);
        return NULL;
    }
    
    // Se for busca por idPessoa, usar busca binária no índice
    if (strcmp(nomeCampo, "idPessoa") == 0) {
        int idProcurado = atoi(valorBusca);
        long long byteOffset;
        
        if (buscaBinariaIndice(indiceArquivo, idProcurado, &byteOffset)) {
            FILE *arquivo = fopen(binArquivo, "rb");
            if (arquivo != NULL) {
                fseek(arquivo, byteOffset, SEEK_SET);
                if (lerRegistroPessoa(arquivo, &resultado->pessoas[0]) && 
                    resultado->pessoas[0].removido == '0') {
                    resultado->posicoes[0] = byteOffset;
                    resultado->quantidade = 1;
                }
                fclose(arquivo);
            }
        }
    } else {
        // Para outros campos, usar busca sequencial
        FILE *arquivo = fopen(binArquivo, "rb");
        if (arquivo == NULL) {
            liberarResultadoBusca(resultado);
            return NULL;
        }
        
        CabecalhoPessoa cabecalho;
        if (fread(&cabecalho.status, sizeof(char), 1, arquivo) != 1 ||
            fread(&cabecalho.quantidadePessoas, sizeof(int), 1, arquivo) != 1 ||
            fread(&cabecalho.quantidadeRemovidos, sizeof(int), 1, arquivo) != 1 ||
            fread(&cabecalho.proxByteOffset, sizeof(long long), 1, arquivo) != 1) {
            fclose(arquivo);
            liberarResultadoBusca(resultado);
            return NULL;
        }
        
        if (cabecalho.status != '1') {
            fclose(arquivo);
            liberarResultadoBusca(resultado);
            return NULL;
        }
        
        long long posicaoAtual;
        while (lerRegistroPessoa(arquivo, &resultado->pessoas[resultado->quantidade])) {
            posicaoAtual = ftell(arquivo) - resultado->pessoas[resultado->quantidade].tamanhoRegistro - 5;
            
            if (resultado->pessoas[resultado->quantidade].removido == '0' &&
                verificarMatch(&resultado->pessoas[resultado->quantidade], nomeCampo, valorBusca)) {
                
                resultado->posicoes[resultado->quantidade] = posicaoAtual;
                resultado->quantidade++;
                
                if (resultado->quantidade >= capacidade) {
                    capacidade *= 2;
                    resultado->pessoas = realloc(resultado->pessoas, capacidade * sizeof(RegistroPessoa));
                    resultado->posicoes = realloc(resultado->posicoes, capacidade * sizeof(long long));
                    if (resultado->pessoas == NULL || resultado->posicoes == NULL) {
                        fclose(arquivo);
                        liberarResultadoBusca(resultado);
                        return NULL;
                    }
                }
            } else {
                // Liberar memória se não for o registro que queremos
                if (resultado->pessoas[resultado->quantidade].nomePessoa) {
                    free(resultado->pessoas[resultado->quantidade].nomePessoa);
                }
                if (resultado->pessoas[resultado->quantidade].nomeUsuario) {
                    free(resultado->pessoas[resultado->quantidade].nomeUsuario);
                }
            }
        }
        
        fclose(arquivo);
    }
    
    return resultado;
}

// Função para liberar memória do resultado de busca
void liberarResultadoBusca(ResultadoBusca *resultado) {
    if (resultado == NULL) return;
    
    if (resultado->pessoas != NULL) {
        for (int i = 0; i < resultado->quantidade; i++) {
            if (resultado->pessoas[i].nomePessoa) {
                free(resultado->pessoas[i].nomePessoa);
            }
            if (resultado->pessoas[i].nomeUsuario) {
                free(resultado->pessoas[i].nomeUsuario);
            }
        }
        free(resultado->pessoas);
    }
    
    if (resultado->posicoes != NULL) {
        free(resultado->posicoes);
    }
    
    free(resultado);
}
