#include "busca.h"
#include "estruturas.h"
#include "registro_pessoa.h"
#include "io_arquivos.h"


// Função auxiliar para busca binária no índice por idPessoa
int buscaBinariaIndice(char *indiceArquivo, int idPessoa, long long *byteOffset) {
    FILE *arquivo;
    CabecalhoIndice cabecalho;
    RegistroIndice registro;
    long long inicio, fim, meio;
    int quantidadeRegistros;
    
    arquivo = fopen(indiceArquivo, "rb");
    if (arquivo == NULL) {
        return 0;
    }
    
    // Ler cabeçalho
    if (fread(&cabecalho.status, sizeof(char), 1, arquivo) != 1 ||
        fread(&cabecalho.lixo, 11, 1, arquivo) != 1) {
        fclose(arquivo);
        return 0;
    }
    
    if (cabecalho.status != '1') {
        fclose(arquivo);
        return 0;
    }
    
    // Calcular quantidade de registros
    fseek(arquivo, 0, SEEK_END);
    long long tamanhoArquivo = ftell(arquivo);
    quantidadeRegistros = (tamanhoArquivo - 12) / 12;
    
    if (quantidadeRegistros <= 0) {
        fclose(arquivo);
        return 0;
    }
    
    // Busca binária
    inicio = 12; // Após cabeçalho
    fim = 12 + (quantidadeRegistros - 1) * 12;
    
    while (inicio <= fim) {
        meio = inicio + ((fim - inicio) / 2);
        meio = (meio / 12) * 12; // Alinhar com início de registro
        
        fseek(arquivo, meio, SEEK_SET);
        if (fread(&registro.idPessoa, sizeof(int), 1, arquivo) != 1 ||
            fread(&registro.byteOffset, sizeof(long long), 1, arquivo) != 1) {
            fclose(arquivo);
            return 0;
        }
        
        if (registro.idPessoa == idPessoa) {
            *byteOffset = registro.byteOffset;
            fclose(arquivo);
            return 1;
        } else if (registro.idPessoa < idPessoa) {
            inicio = meio + 12;
        } else {
            fim = meio - 12;
        }
    }
    
    fclose(arquivo);
    return 0;
}

// função auxiliar para busca sequencial
int buscaSequencial(char *binArquivo, char *nomeCampo, char *valorBusca) {
    FILE *arquivo;
    CabecalhoPessoa cabecalho;
    RegistroPessoa pessoa;
    int encontrados = 0;

    arquivo = fopen(binArquivo, "rb");
    if (arquivo == NULL) {
        printf("Falha no processamento do arquivo.\n");
        exit (0);
    }

    if ( fread(&cabecalho.status, sizeof(char), 1, arquivo) != 1 ||
    fread(&cabecalho.quantidadePessoas, sizeof(int), 1, arquivo) != 1 ||
    fread(&cabecalho.quantidadeRemovidos, sizeof(int), 1, arquivo) != 1 ||
    fread(&cabecalho.proxByteOffset, sizeof(long long), 1, arquivo) != 1
) {
        printf("Falha no processamento do arquivo.\n");
        fclose(arquivo);
        exit (0);
    }

    if (cabecalho.status != '1') {
        printf("Falha no processamento do arquivo.\n");
        fclose(arquivo);
        exit (0);
    }

    while (lerRegistroPessoa(arquivo, &pessoa)) {
        if (pessoa.removido == '0') {
            int match = 0;

            if (strcmp(nomeCampo, "idPessoa") == 0) {
                int idProcurado = atoi(valorBusca);
                if (pessoa.idPessoa == idProcurado) {
                    match = 1;
                }
            } else if (strcmp(nomeCampo, "nomePessoa") == 0) {
                if (pessoa.nomePessoa && strcmp(pessoa.nomePessoa, valorBusca) == 0) {
                    match = 1;
                } else if (!pessoa.nomePessoa && strlen(valorBusca) == 0) {
                    match = 1;
                }
            } else if (strcmp(nomeCampo, "idade") == 0 || strcmp(nomeCampo, "idadePessoa") == 0) {
                int idadeProcurada = atoi(valorBusca);
                if (pessoa.idadePessoa == idadeProcurada) {
                    match = 1;
                }
            } else if (strcmp(nomeCampo, "nomeUsuario") == 0) {
                if (pessoa.nomeUsuario && strcmp(pessoa.nomeUsuario, valorBusca) == 0) {
                    match = 1;
                } else if (!pessoa.nomeUsuario && strlen(valorBusca) == 0) {
                    match = 1;
                }
            }

            if (match) {
                printf("Dados da pessoa de codigo %d\n", pessoa.idPessoa);
                printf("Nome: %s\n", pessoa.nomePessoa ? pessoa.nomePessoa : "-");
                if (pessoa.idadePessoa == -1) {
                    printf("Idade: -\n");
                } else {
                    printf("Idade: %d\n", pessoa.idadePessoa);
                }
                printf("Usuario: %s\n", pessoa.nomeUsuario ? pessoa.nomeUsuario : "-");
                printf("\n");
                encontrados++;
            }
        }

        if (pessoa.nomePessoa) {
            free(pessoa.nomePessoa);
            pessoa.nomePessoa = NULL;
        }
        if (pessoa.nomeUsuario) {
            free(pessoa.nomeUsuario);
            pessoa.nomeUsuario = NULL;
        }
    }

    fclose(arquivo);
    return encontrados;
}



// Função auxiliar para buscar e marcar registros para remoção
int buscarEMarcarRemocao(char *binArquivo, char *nomeCampo, char *valorBusca) {
    FILE *arquivo;
    CabecalhoPessoa cabecalho;
    RegistroPessoa pessoa;
    int registrosRemovidos = 0;
    long long posicaoAtual;

    arquivo = fopen(binArquivo, "r+b");
    if (arquivo == NULL) {
        printf("Falha no processamento do arquivo.\n");
        exit (0);
    }

    // Ler cabeçalho
    if (fread(&cabecalho.status, sizeof(char), 1, arquivo) != 1 ||
        fread(&cabecalho.quantidadePessoas, sizeof(int), 1, arquivo) != 1 ||
        fread(&cabecalho.quantidadeRemovidos, sizeof(int), 1, arquivo) != 1 ||
        fread(&cabecalho.proxByteOffset, sizeof(long long), 1, arquivo) != 1) {
        printf("Falha no processamento do arquivo.\n");
        fclose(arquivo);
        exit (0);
    }

    if (cabecalho.status != '1') {
        printf("Falha no processamento do arquivo.\n");
        fclose(arquivo);
        exit (0);
    }

    // Marcar arquivo como inconsistente durante modificação
    cabecalho.status = '0';
    fseek(arquivo, 0, SEEK_SET);
    fwrite(&cabecalho.status, sizeof(char), 1, arquivo);
    fseek(arquivo, 17, SEEK_SET); // Voltar para início dos registros

    // Percorrer todos os registros sequencialmente
    while (lerRegistroPessoa(arquivo, &pessoa)) {
        if (pessoa.removido == '0') { // Se não está removido
            int match = 0;

            // Verificar critério de busca
            if (strcmp(nomeCampo, "idPessoa") == 0) {
                int idProcurado = atoi(valorBusca);
                if (pessoa.idPessoa == idProcurado) {
                    match = 1;
                }
            } else if (strcmp(nomeCampo, "nomePessoa") == 0) {
                if (strcmp(valorBusca, "NULO") == 0) {
                    if (pessoa.nomePessoa == NULL || strlen(pessoa.nomePessoa) == 0) {
                        match = 1;
                    }
                } else if (pessoa.nomePessoa && strcmp(pessoa.nomePessoa, valorBusca) == 0) {
                    match = 1;
                }
            } else if (strcmp(nomeCampo, "idade") == 0 || strcmp(nomeCampo, "idadePessoa") == 0) {
                if (strcmp(valorBusca, "NULO") == 0) {
                    if (pessoa.idadePessoa == -1) {
                        match = 1;
                    }
                } else {
                    int idadeProcurada = atoi(valorBusca);
                    if (pessoa.idadePessoa == idadeProcurada) {
                        match = 1;
                    }
                }
            } else if (strcmp(nomeCampo, "nomeUsuario") == 0) {
                if (strcmp(valorBusca, "NULO") == 0) {
                    if (pessoa.nomeUsuario == NULL || strlen(pessoa.nomeUsuario) == 0) {
                        match = 1;
                    }
                } else if (pessoa.nomeUsuario && strcmp(pessoa.nomeUsuario, valorBusca) == 0) {
                    match = 1;
                }
            }

            if (match) {
                // Marcar como removido
                posicaoAtual = ftell(arquivo) - pessoa.tamanhoRegistro - 5; // Posição do campo removido
                fseek(arquivo, posicaoAtual, SEEK_SET);
                char removido = '1';
                fwrite(&removido, sizeof(char), 1, arquivo);
                fseek(arquivo, 0, SEEK_END); // Voltar para o final
                
                registrosRemovidos++;
                cabecalho.quantidadeRemovidos++;
            }
        }

        // Liberar memória
        if (pessoa.nomePessoa) {
            free(pessoa.nomePessoa);
            pessoa.nomePessoa = NULL;
        }
        if (pessoa.nomeUsuario) {
            free(pessoa.nomeUsuario);
            pessoa.nomeUsuario = NULL;
        }
    }

    // Atualizar cabeçalho
    cabecalho.status = '1';
    fseek(arquivo, 0, SEEK_SET);
    fwrite(&cabecalho.status, sizeof(char), 1, arquivo);
    fwrite(&cabecalho.quantidadePessoas, sizeof(int), 1, arquivo);
    fwrite(&cabecalho.quantidadeRemovidos, sizeof(int), 1, arquivo);
    fwrite(&cabecalho.proxByteOffset, sizeof(long long), 1, arquivo);

    fclose(arquivo);
    return registrosRemovidos;
}


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
            } else{
                printf("Falha no processamento do arquivo.\n");
                exit(0);
            }
        }
    } else {
        // Para outros campos, usar busca sequencial
        FILE *arquivo = fopen(binArquivo, "rb");
        if (arquivo == NULL) {
            printf("Falha no processamento do arquivo.\n");
            liberarResultadoBusca(resultado);
            exit(0);
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


// Função auxiliar para buscar registros que atendem ao critério
int buscarRegistrosParaAtualizacao(char *binArquivo, char *nomeCampo, char *valorBusca, long long **posicoes, int *quantidade) {
    FILE *arquivo;
    CabecalhoPessoa cabecalho;
    RegistroPessoa pessoa;
    long long *posicoesEncontradas = NULL;
    int capacidade = 100;
    int tamanho = 0;
    long long posicaoAtual;

    arquivo = fopen(binArquivo, "rb");
    if (arquivo == NULL) {
        printf("Falha no processamento do arquivo.\n");
        exit(0);
    }

    // Ler cabeçalho
    if (fread(&cabecalho.status, sizeof(char), 1, arquivo) != 1 ||
        fread(&cabecalho.quantidadePessoas, sizeof(int), 1, arquivo) != 1 ||
        fread(&cabecalho.quantidadeRemovidos, sizeof(int), 1, arquivo) != 1 ||
        fread(&cabecalho.proxByteOffset, sizeof(long long), 1, arquivo) != 1) {
        printf("Falha no processamento do arquivo.\n");
        fclose(arquivo);
        exit(0);
    }

    if (cabecalho.status != '1') {
        printf("Falha no processamento do arquivo.\n");
        fclose(arquivo);
        exit(0);
    }

    posicoesEncontradas = malloc(capacidade * sizeof(long long));
    if (posicoesEncontradas == NULL) {
        printf("Falha no processamento do arquivo.\n");
        fclose(arquivo);
        exit(0);
    }

    // Percorrer todos os registros
    while (lerRegistroPessoa(arquivo, &pessoa)) {
        posicaoAtual = ftell(arquivo) - pessoa.tamanhoRegistro - 5; // Posição do início do registro
        
        if (pessoa.removido == '0') {
            int match = 0;

            // Verificar critério de busca
            if (strcmp(nomeCampo, "idPessoa") == 0) {
                int idProcurado = atoi(valorBusca);
                if (pessoa.idPessoa == idProcurado) {
                    match = 1;
                }
            } else if (strcmp(nomeCampo, "nomePessoa") == 0) {
                if (strcmp(valorBusca, "NULO") == 0) {
                    if (pessoa.nomePessoa == NULL || strlen(pessoa.nomePessoa) == 0) {
                        match = 1;
                    }
                } else if (pessoa.nomePessoa && strcmp(pessoa.nomePessoa, valorBusca) == 0) {
                    match = 1;
                }
            } else if (strcmp(nomeCampo, "idade") == 0 || strcmp(nomeCampo, "idadePessoa") == 0) {
                if (strcmp(valorBusca, "NULO") == 0) {
                    if (pessoa.idadePessoa == -1) {
                        match = 1;
                    }
                } else {
                    int idadeProcurada = atoi(valorBusca);
                    if (pessoa.idadePessoa == idadeProcurada) {
                        match = 1;
                    }
                }
            } else if (strcmp(nomeCampo, "nomeUsuario") == 0) {
                if (strcmp(valorBusca, "NULO") == 0) {
                    if (pessoa.nomeUsuario == NULL || strlen(pessoa.nomeUsuario) == 0) {
                        match = 1;
                    }
                } else if (pessoa.nomeUsuario && strcmp(pessoa.nomeUsuario, valorBusca) == 0) {
                    match = 1;
                }
            }

            if (match) {
                if (tamanho >= capacidade) {
                    capacidade *= 2;
                    posicoesEncontradas = realloc(posicoesEncontradas, capacidade * sizeof(long long));
                    if (posicoesEncontradas == NULL) {
                        printf("Falha no processamento do arquivo.\n");
                        fclose(arquivo);
                        exit(0);
                    }
                }
                posicoesEncontradas[tamanho] = posicaoAtual;
                tamanho++;
            }
        }

        // Liberar memória
        if (pessoa.nomePessoa) {
            free(pessoa.nomePessoa);
            pessoa.nomePessoa = NULL;
        }
        if (pessoa.nomeUsuario) {
            free(pessoa.nomeUsuario);
            pessoa.nomeUsuario = NULL;
        }
    }

    fclose(arquivo);
    *posicoes = posicoesEncontradas;
    *quantidade = tamanho;
    return tamanho;
}


// Função de busca binária modificada conforme orientações
int BuscaBinariaSegue(RegistroSegue **registros, int tamanho, int idPessoaQueSegue) {
    int inicio = 0;
    int fim = tamanho - 1;
    int meio;
    
    while (inicio <= fim) {
        meio = (inicio + fim) / 2;
        
        if (registros[meio]->idPessoaQueSegue == idPessoaQueSegue) {
            return meio; // Encontrou um registro com o ID
        } else if (registros[meio]->idPessoaQueSegue < idPessoaQueSegue) {
            inicio = meio + 1;
        } else {
            fim = meio - 1;
        }
    }
    
    return -1; // Não encontrou
}

// Função auxiliar para busca binária no arquivo segueOrdenado
int buscaBinariaSegue(FILE *arquivo, int idPessoaQueSegue, RegistroSegue *resultados, int *quantidadeResultados) {
    CabecalhoSegue cabecalho;
    int inicio, fim, meio;
    RegistroSegue registro;
    int encontrados = 0;
    int capacidade = 100;
    
    // Ler cabeçalho
    if (fread(&cabecalho.status, sizeof(char), 1, arquivo) != 1 ||
        fread(&cabecalho.quantidadeSegue, sizeof(int), 1, arquivo) != 1 ||
        fread(&cabecalho.proxRRN, sizeof(int), 1, arquivo) != 1) {
        return 0;
    }
    
    if (cabecalho.status != '1') {
        return 0;
    }
    
    // Busca binária
    inicio = 0; //primeiro RRN válido
    fim = cabecalho.proxRRN ; //último RRN válido
    long long deslocamento; // Deslocamento para fseek
    
    while (inicio <= fim) {
        meio = (inicio + fim) / 2;
        deslocamento = 9 + meio * 30; // 9 bytes de cabeçalho + tamanho fixo do registro segue
        fseek(arquivo, meio, SEEK_SET);
        
        // Ler registro na posição meio
        if (!lerRegistroSegue(arquivo, &registro)) {
            break;
        }
        
        if (registro.idPessoaQueSegue == idPessoaQueSegue) {
            // Encontrou um registro, agora buscar todos os registros com o mesmo idPessoaQueSegue
            // Primeiro, encontrar o primeiro registro com esse ID
            long long posicaoAtual = ftell(arquivo) - 30; // Tamanho fixo do registro segue
        
            
            // Buscar para trás para encontrar o primeiro registro
            while (posicaoAtual > 17) {
                fseek(arquivo, posicaoAtual - 1, SEEK_SET);
                RegistroSegue registroAnterior;
                if (!lerRegistroSegue(arquivo, &registroAnterior)) {
                    break;
                }
                if (registroAnterior.idPessoaQueSegue != idPessoaQueSegue) {
                   // if (registroAnterior.dataInicioQueSegue) free(registroAnterior.dataInicioQueSegue);
                   // if (registroAnterior.dataFimQueSegue) free(registroAnterior.dataFimQueSegue);
                    break;
                }
                posicaoAtual = ftell(arquivo) - 30;
               // if (registroAnterior.dataInicioQueSegue) free(registroAnterior.dataInicioQueSegue);
               // if (registroAnterior.dataFimQueSegue) free(registroAnterior.dataFimQueSegue);
            }
            
            // Agora ler todos os registros consecutivos com o mesmo ID
            fseek(arquivo, posicaoAtual, SEEK_SET);
            while (lerRegistroSegue(arquivo, &registro)) {
                if (registro.idPessoaQueSegue != idPessoaQueSegue) {
                  //  if (registro.dataInicioQueSegue) free(registro.dataInicioQueSegue);
                  //  if (registro.dataFimQueSegue) free(registro.dataFimQueSegue);
                    break;
                }
                
                if (encontrados >= capacidade) {
                    capacidade *= 2;
                    resultados = realloc(resultados, capacidade * sizeof(RegistroSegue));
                    if (resultados == NULL) {
                     //   if (registro.dataInicioQueSegue) free(registro.dataInicioQueSegue);
                     //   if (registro.dataFimQueSegue) free(registro.dataFimQueSegue);
                        return 0;
                    }
                }
                
                resultados[encontrados] = registro;
                encontrados++;
            }
            break;
        } else if (registro.idPessoaQueSegue < idPessoaQueSegue) {
            inicio = meio + 1;
        } else {
            fim = meio - 1;
        }
        
      //  if (registro.dataInicioQueSegue) free(registro.dataInicioQueSegue);
      //  if (registro.dataFimQueSegue) free(registro.dataFimQueSegue);
    }
    
    *quantidadeResultados = encontrados;
    return encontrados;
}

// Função auxiliar para buscar pessoa usando funcionalidade 4
int buscarPessoaParaJoin(char *binArquivo, char *nomeCampo, char *valorBusca, RegistroPessoa *pessoa) {
    FILE *arquivo;
    CabecalhoPessoa cabecalho;
    int encontrado = 0;
    
    arquivo = fopen(binArquivo, "rb");
    if (arquivo == NULL) {
        return 0;
    }
    
    if (fread(&cabecalho.status, sizeof(char), 1, arquivo) != 1 ||
        fread(&cabecalho.quantidadePessoas, sizeof(int), 1, arquivo) != 1 ||
        fread(&cabecalho.quantidadeRemovidos, sizeof(int), 1, arquivo) != 1 ||
        fread(&cabecalho.proxByteOffset, sizeof(long long), 1, arquivo) != 1) {
        fclose(arquivo);
        return 0;
    }
    
    if (cabecalho.status != '1') {
        fclose(arquivo);
        return 0;
    }
    
    while (lerRegistroPessoa(arquivo, pessoa)) {
        if (pessoa->removido == '0') {
            int match = 0;
            
            if (strcmp(nomeCampo, "idPessoa") == 0) {
                int idProcurado = atoi(valorBusca);
                if (pessoa->idPessoa == idProcurado) {
                    match = 1;
                }
            } else if (strcmp(nomeCampo, "nomePessoa") == 0) {
                if (strcmp(valorBusca, "NULO") == 0) {
                    if (pessoa->nomePessoa == NULL || strlen(pessoa->nomePessoa) == 0) {
                        match = 1;
                    }
                } else if (pessoa->nomePessoa && strcmp(pessoa->nomePessoa, valorBusca) == 0) {
                    match = 1;
                }
            } else if (strcmp(nomeCampo, "idade") == 0 || strcmp(nomeCampo, "idadePessoa") == 0) {
                if (strcmp(valorBusca, "NULO") == 0) {
                    if (pessoa->idadePessoa == -1) {
                        match = 1;
                    }
                } else {
                    int idadeProcurada = atoi(valorBusca);
                    if (pessoa->idadePessoa == idadeProcurada) {
                        match = 1;
                    }
                }
            } else if (strcmp(nomeCampo, "nomeUsuario") == 0) {
                if (strcmp(valorBusca, "NULO") == 0) {
                    if (pessoa->nomeUsuario == NULL || strlen(pessoa->nomeUsuario) == 0) {
                        match = 1;
                    }
                } else if (pessoa->nomeUsuario && strcmp(pessoa->nomeUsuario, valorBusca) == 0) {
                    match = 1;
                }
            }
            
            if (match) {
                encontrado = 1;
                break;
            }
        }
        
        // Liberar memória se não for o registro encontrado
        if (!encontrado) {
            if (pessoa->nomePessoa) {
                free(pessoa->nomePessoa);
                pessoa->nomePessoa = NULL;
            }
            if (pessoa->nomeUsuario) {
                free(pessoa->nomeUsuario);
                pessoa->nomeUsuario = NULL;
            }
        }
    }

    
    fclose(arquivo);
    return encontrado;
}
