#include "crud_pessoa.h"
#include "io_arquivos.h"
#include "registro_pessoa.h"
#include "indice.h"
#include "validacao.h"
#include "csv_parser.h"
#include "busca.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define debug 0
#define debugi 0
#define debugj 0

// função 01 - Criar índice
void CREAT_INDEX(char *nomeArquivo) {
    criarIndice(nomeArquivo);
}

// função 02 - Criar tabela Pessoa
void CREAT_TABLE(char *csvArquivo, char *binArquivo, char *indiceArquivo) {
    FILE *csv, *binFile, *indiceFile;
    CabecalhoPessoa cabecalhoPessoa;
    CabecalhoIndice cabecalhoIndice;
    RegistroPessoa pessoa;
    RegistroIndice registroIndice;
    int quantidadePessoas = 0;
    long long offsetAtual;

    csv = fopen(csvArquivo, "r");
    if (csv == NULL) {
        printf("Falha no processamento do arquivo.\n");
        exit (0);
    }

    binFile = fopen(binArquivo, "wb");
    if (binFile == NULL) {
        printf("Falha no processamento do arquivo.\n");
        fclose(csv);
        exit (0);
    }

    indiceFile = fopen(indiceArquivo, "r+b");
    if (indiceFile == NULL) {
        indiceFile = fopen(indiceArquivo, "w+b");
        if (indiceFile == NULL) {
            printf("Falha no processamento do arquivo.\n");
            fclose(csv);
            fclose(binFile);
            exit (0);
        }

        inicializarCabecalhoIndice(&cabecalhoIndice);
        escreverCabecalhoIndice(indiceFile, &cabecalhoIndice);
    } else {
        lerCabecalhoIndice(indiceFile, &cabecalhoIndice);
        cabecalhoIndice.status = '0';
        fseek(indiceFile, 0, SEEK_SET);
        escreverCabecalhoIndice(indiceFile, &cabecalhoIndice);
        fseek(indiceFile, 12, SEEK_SET);
    }

    inicializarCabecalhoPessoa(&cabecalhoPessoa);
    escreverCabecalhoPessoa(binFile, &cabecalhoPessoa);

    // pula a primeira linha do CSV
    pularCabecalhoCSV(csv);

    // Primeira passada: coletar todos os registros e escrever no arquivo pessoa
    RegistroIndice *registrosIndice = NULL;
    int capacidade = 100;
    int tamanhoAtual = 0;

    registrosIndice = malloc(capacidade * sizeof(RegistroIndice));
    if (registrosIndice == NULL) {
        printf("Falha no processamento do arquivo.\n");
        fclose(csv);
        fclose(binFile);
        fclose(indiceFile);
        exit (0);
    }

    // ler cada linha do CSV
    while (ler_csv(csv, &pessoa)) {
        offsetAtual = ftell(binFile);
        escreverRegistroPessoa(binFile, &pessoa);

        if (tamanhoAtual >= capacidade) {
            capacidade *= 2;
            registrosIndice = realloc(registrosIndice, capacidade * sizeof(RegistroIndice));
            if (registrosIndice == NULL) {
                printf("Falha no processamento do arquivo.\n");
                fclose(csv);
                fclose(binFile);
                fclose(indiceFile);
                exit (0);
            }
        }

        registrosIndice[tamanhoAtual].idPessoa = pessoa.idPessoa;
        registrosIndice[tamanhoAtual].byteOffset = offsetAtual;
        tamanhoAtual++;
        quantidadePessoas++;

        liberarRegistroPessoa(&pessoa);
    }
    
    // ordena os registros de índice por idPessoa em ordem crescente
    qsort(registrosIndice, tamanhoAtual, sizeof(RegistroIndice), compararIdPessoa);

    for (int i = 0; i < tamanhoAtual; i++) {
        fwrite(&registrosIndice[i].idPessoa, sizeof(int), 1, indiceFile);
        fwrite(&registrosIndice[i].byteOffset, sizeof(long long), 1, indiceFile);
    }

    // Liberar memória do array de índices
    free(registrosIndice);

    // Atualizar cabeçalho do arquivo pessoa
    cabecalhoPessoa.status = '1';
    cabecalhoPessoa.quantidadePessoas = quantidadePessoas;
    cabecalhoPessoa.proxByteOffset = ftell(binFile);

    fseek(binFile, 0, SEEK_SET);
    escreverCabecalhoPessoa(binFile, &cabecalhoPessoa);

    cabecalhoIndice.status = '1';
    fseek(indiceFile, 0, SEEK_SET);
    escreverCabecalhoIndice(indiceFile, &cabecalhoIndice);

    fclose(csv);
    fclose(binFile);
    fclose(indiceFile);

    // Mostrar conteúdo dos arquivos
    binarioNaTela(binArquivo);
    binarioNaTela(indiceArquivo);
}

// função 3: Listar todos os registros do arquivo binario
void SELECT(char *binArquivo) {
    FILE *arquivo;
    CabecalhoPessoa cabecalho;
    RegistroPessoa pessoa;
    int registrosEncontrados = 0;

    arquivo = fopen(binArquivo, "rb");
    if (arquivo == NULL) {
        printf("Falha no processamento do arquivo.\n");
        exit (0);
    }

    if (!lerCabecalhoPessoa(arquivo, &cabecalho)) {
        printf("Falha no processamento do arquivo.\n");
        fclose(arquivo);
        exit (0);
    }

    // confere se o arquivo está consistente
    if (!validarArquivoConsistente(cabecalho.status)) {
        printf("Falha no processamento do arquivo.\n");
        fclose(arquivo);
        exit (0);
    }

    // Percorrer todos os registros sequencialmente
    while (lerRegistroPessoa(arquivo, &pessoa)) {
        if (pessoa.removido == '0') {
            registrosEncontrados++;
            imprimirRegistroPessoa(&pessoa);
            printf("\n");
        }

        liberarRegistroPessoa(&pessoa);
    }

    // Se não encontrou nenhum registro valido
    if (registrosEncontrados == 0) {
        printf("Registro inexistente.\n");
    }

    fclose(arquivo);
}

// função 04 - SELECT WHERE
void SELECT_WHERE(char *binArquivo, char *indiceArquivo, int n) {
    int totalEncontrados = 0;

    for (int i = 0; i < n; i++) {
        char entrada[300];
        char nomeCampo[100];
        char valorBusca[200];

        if (fgets(entrada, sizeof(entrada), stdin) == NULL) {
            printf("Falha no processamento do arquivo.\n");
            exit (0);
        }

        entrada[strcspn(entrada, "\r\n")] = '\0';

        if (strlen(entrada) == 0) {
            i--;
            continue;
        }

        // Procura simbolo = para separar campo e valor
        char *separador = strchr(entrada, '=');
        if (separador == NULL) {
            printf("Falha no processamento do arquivo.\n");
            exit (0);
        }

        // Separ nomeCampo e valorCampo
        *separador = '\0';
        char *nomeCampoPtr = entrada;

        while (*nomeCampoPtr && (isdigit(*nomeCampoPtr) || isspace(*nomeCampoPtr))) {
            nomeCampoPtr++;
        }

        strcpy(nomeCampo, nomeCampoPtr);
        strcpy(valorBusca, separador + 1);

        // Se tiver entre aspas remover as aspas
        removerAspas(valorBusca);

        // USAR MÓDULO DE BUSCA MODULARIZADO
        ResultadoBusca *resultado = buscarPessoas(binArquivo, indiceArquivo, nomeCampo, valorBusca);
        int encontrados = 0;
        
        if (resultado != NULL && resultado->quantidade > 0) {
            for (int j = 0; j < resultado->quantidade; j++) {
                imprimirRegistroPessoa(&resultado->pessoas[j]);
                printf("\n");
            }
            encontrados = resultado->quantidade;
        }
        
        // Liberar memória do resultado
        if (resultado != NULL) {
            liberarResultadoBusca(resultado);
        }

        totalEncontrados += encontrados;
        if (encontrados == 0) printf("Registro inexistente.\n\n");
    }
}

// Função auxiliar para inserir registro no arquivo
int inserirRegistroPessoa(char *binArquivo, RegistroPessoa *pessoa) {
    FILE *arquivo;
    CabecalhoPessoa cabecalho;
    long long posicaoInsercao;

    arquivo = fopen(binArquivo, "r+b");
    if (arquivo == NULL) {
        printf("Falha no processamento do arquivo.\n");
        exit(0);
    }

    // Ler cabeçalho
    if (!lerCabecalhoPessoa(arquivo, &cabecalho)) {
        printf("Falha no processamento do arquivo.\n");
        fclose(arquivo);
        exit(0);
    }

    if (!validarArquivoConsistente(cabecalho.status)) {
        printf("Falha no processamento do arquivo.\n");
        fclose(arquivo);
        exit(0);
    }

    // Marcar arquivo como inconsistente durante modificação
    marcarArquivoInconsistente(arquivo, 0); // 0 = Pessoa

    // Inserir no final do arquivo
    fseek(arquivo, 0, SEEK_END);
    posicaoInsercao = ftell(arquivo);
    
    // Escrever registro
    escreverRegistroPessoa(arquivo, pessoa);

    // Atualizar cabeçalho
    cabecalho.quantidadePessoas++;
    cabecalho.proxByteOffset = ftell(arquivo);
    cabecalho.status = '1';

    fseek(arquivo, 0, SEEK_SET);
    escreverCabecalhoPessoa(arquivo, &cabecalho);

    fclose(arquivo);
    return posicaoInsercao;
}

// Função 6 - INSERT INTO
void INSERT_INTO(char *binArquivo, char *indiceArquivo) {
    int n;
    scanf("%d", &n);
    RegistroPessoa pessoa;

    for (int i = 0; i < n; i++) {
        char linha[500];
        char campos[4][200];
        int numCampos = 0;
        
        long long byteOffset;

        // Ler linha de entrada
        if (fgets(linha, sizeof(linha), stdin) == NULL) {
            printf("Falha no processamento do arquivo.\n");
            exit(0);
        }

        linha[strcspn(linha, "\r\n")] = '\0';

        if (strlen(linha) == 0) {
            i--;
            continue;
        }

        // Pular número da linha (ex: "1 ", "2 ", etc.)
        char *ptr = pularNumeroLinha(linha);

        // Separar campos por vírgula
        char *token = strtok(ptr, ",");
        while (token != NULL && numCampos < 4) {
            // Remover espaços em branco do início e fim
            while (*token == ' ') token++;
            char *end = token + strlen(token) - 1;
            while (end > token && *end == ' ') end--;
            *(end + 1) = '\0';

            strcpy(campos[numCampos], token);
            numCampos++;
            token = strtok(NULL, ",");
        }

        if (numCampos < 4) {
            printf("Falha no processamento do arquivo.\n");
            exit(0);
        }


        inicializarRegistroPessoa(&pessoa);

        // Processar idPessoa
        pessoa.idPessoa = atoi(campos[0]);

        // Verificar se ID já existe
        if (verificarIdExistente(binArquivo, pessoa.idPessoa)) {
            printf("Falha no processamento do arquivo.\n");
            exit(0);
        }

        

        // Processar nomePessoa
        if (strcmp(campos[1], "NULO") == 0) {
            pessoa.tamanhoNomePessoa = 0;
            pessoa.nomePessoa = NULL;
        } else {
            removerAspas(campos[1]);
            pessoa.tamanhoNomePessoa = strlen(campos[1]);
            pessoa.nomePessoa = malloc(pessoa.tamanhoNomePessoa + 1);
            if (pessoa.nomePessoa == NULL) {
                printf("Falha no processamento do arquivo.\n");
                exit(0);
            }
            strcpy(pessoa.nomePessoa, campos[1]);
        }

        // Processar idadePessoa
        if (strcmp(campos[2], "NULO") == 0) {
            pessoa.idadePessoa = -1;
        } else {
            pessoa.idadePessoa = atoi(campos[2]);
        }

        // Processar nomeUsuario
        if (strcmp(campos[3], "NULO") == 0) {
            pessoa.tamanhoNomeUsuario = 0;
            pessoa.nomeUsuario = NULL;
        } else {
            removerAspas(campos[3]);
            pessoa.tamanhoNomeUsuario = strlen(campos[3]);
            pessoa.nomeUsuario = malloc(pessoa.tamanhoNomeUsuario + 1);
            if (pessoa.nomeUsuario == NULL) {
                if (pessoa.nomePessoa) free(pessoa.nomePessoa);
                printf("Falha no processamento do arquivo.\n");
                exit(0);
            }
            strcpy(pessoa.nomeUsuario, campos[3]);
        }

        pessoa.tamanhoRegistro = 0; // Será calculado na função de escrita
        
        // Inserir registro no arquivo
        byteOffset = inserirRegistroPessoa(binArquivo, &pessoa);

        // Atualizar índice
        atualizarIndice(indiceArquivo, pessoa.idPessoa, byteOffset);

        // Liberar memória
        liberarRegistroPessoa(&pessoa);
    }

    // Mostrar arquivos binários na tela
    binarioNaTela(binArquivo);
    binarioNaTela(indiceArquivo);
}

// Função 5 - DELETE FROM
void DELETE_FROM(char *binArquivo, char *indiceArquivo) {
    int n;
    scanf("%d", &n);
    
    // Carregar índice em memória
    int tamanhoIndice;
    RegistroIndice *registrosIndice = CarregarIndiceEmMemoria(indiceArquivo, &tamanhoIndice);

    if (registrosIndice == NULL) {
        printf("Falha no processamento do arquivo.\n");
        exit(0);
    }
    
    FILE *arquivo;

    if(binArquivo == NULL || !(arquivo = fopen(binArquivo, "rb"))) {
        printf("Falha no processamento do arquivo.\n");
        exit (0);
    } else fclose(arquivo);

    arquivo = fopen(binArquivo, "r+b");
    for (int i = 0; i < n; i++) {
        char entrada[300];
        char nomeCampo[100];
        char valorBusca[200];
        
        if (fgets(entrada, sizeof(entrada), stdin) == NULL) {
            printf("Falha no processamento do arquivo.\n");
            free(registrosIndice);
            exit(0);
        }
        
        entrada[strcspn(entrada, "\r\n")] = '\0';
        
        if (strlen(entrada) == 0) {
            i--;
            continue;
        }
        
        // Pular número da linha (ex: "1 ", "2 ", etc.)
        char *ptr = pularNumeroLinha(entrada);

        // Procura símbolo = para separar campo e valor
        char *separador = strchr(ptr, '=');
        if (separador == NULL) {
            printf("Falha no processamento do arquivo.\n");
            free(registrosIndice);
            fclose(arquivo);
            exit(0);
        }
        
        // Separar nomeCampo e valorCampo
        *separador = '\0';
        strcpy(nomeCampo, ptr);
        strcpy(valorBusca, separador + 1);
        
        // Se tiver entre aspas, remover as aspas
        removerAspas(valorBusca);
        
        // USAR MÓDULO DE BUSCA MODULARIZADO
        ResultadoBusca *resultado = buscarPessoas(binArquivo, indiceArquivo, nomeCampo, valorBusca);
        
        if (resultado != NULL && resultado->quantidade > 0) {
            // Marcar arquivo como inconsistente
            marcarArquivoInconsistente(arquivo, 0); // 0 = Pessoa
            
            // Marcar cada registro encontrado como removido
            for (int j = 0; j < resultado->quantidade; j++) {
                marcarRegistroRemovido(arquivo, resultado->posicoes[j]);
                
                // Remover do índice em memória
                RemoverDoIndice(&registrosIndice, &tamanhoIndice, resultado->pessoas[j].idPessoa);
            }
            
            // Atualizar cabeçalho
            CabecalhoPessoa cabecalho;
            fseek(arquivo, 0, SEEK_SET);
            lerCabecalhoPessoa(arquivo, &cabecalho);
            cabecalho.quantidadeRemovidos += resultado->quantidade;
            cabecalho.quantidadePessoas -= resultado->quantidade;
            cabecalho.status = '1';
            fseek(arquivo, 0, SEEK_SET);
            escreverCabecalhoPessoa(arquivo, &cabecalho);

            fflush(arquivo);

        }
        
        // Liberar memória do resultado
        if (resultado != NULL) {
            liberarResultadoBusca(resultado);
        }
    }
    
    fclose(arquivo);
    
    // Reescrever índice
    ReescreverIndice(indiceArquivo, registrosIndice, tamanhoIndice);
    free(registrosIndice);
    
    // Mostrar arquivos binários na tela
    binarioNaTela(binArquivo);
    binarioNaTela(indiceArquivo);
}

// Função auxiliar para aplicar atualizações em um registro
int aplicarAtualizacoes(RegistroPessoa *pessoa, char *nomeCampo, char *valorAtualizacao) {
    if (strcmp(nomeCampo, "idPessoa") == 0) {
        // permitir atualização de idPessoa
        pessoa->idPessoa = atoi(valorAtualizacao);

        return 1; // Indica tentativa de alterar o ID
    } else if (strcmp(nomeCampo, "nomePessoa") == 0) {
        if (strcmp(valorAtualizacao, "NULO") == 0) {
            if (pessoa->nomePessoa) {
                free(pessoa->nomePessoa);
            }
            pessoa->nomePessoa = NULL;
            pessoa->tamanhoNomePessoa = 0;
        } else {
            removerAspas(valorAtualizacao);
            if (pessoa->nomePessoa) {
                free(pessoa->nomePessoa);
            }
            pessoa->tamanhoNomePessoa = strlen(valorAtualizacao);
            pessoa->nomePessoa = malloc(pessoa->tamanhoNomePessoa + 1);
            if (pessoa->nomePessoa == NULL) {
                printf("Falha no processamento do arquivo.\n");
                exit(0);
            }
            strcpy(pessoa->nomePessoa, valorAtualizacao);
        }
    } else if (strcmp(nomeCampo, "idade") == 0 || strcmp(nomeCampo, "idadePessoa") == 0) {
        if (strcmp(valorAtualizacao, "NULO") == 0) {
            pessoa->idadePessoa = -1;
        } else {
            pessoa->idadePessoa = atoi(valorAtualizacao);
        }
    } else if (strcmp(nomeCampo, "nomeUsuario") == 0) {
        if (strcmp(valorAtualizacao, "NULO") == 0) {
            if (pessoa->nomeUsuario) {
                free(pessoa->nomeUsuario);
            }
            pessoa->nomeUsuario = NULL;
            pessoa->tamanhoNomeUsuario = 0;
        } else {
            removerAspas(valorAtualizacao);
            if (pessoa->nomeUsuario) {
                free(pessoa->nomeUsuario);
            }
            pessoa->tamanhoNomeUsuario = strlen(valorAtualizacao);
            pessoa->nomeUsuario = malloc(pessoa->tamanhoNomeUsuario + 1);
            if (pessoa->nomeUsuario == NULL) {
                printf("Falha no processamento do arquivo.\n");
                exit(0);
            }
            strcpy(pessoa->nomeUsuario, valorAtualizacao);
        }
    }
    return 0; // Indica que não houve tentativa de alterar o ID
}

// Função 7 - UPDATE
void UPDATE(char *binArquivo, char *indiceArquivo) {
    int n;
    scanf("%d", &n);

    for (int i = 0; i < n; i++) {
        char entrada[500];
        char nomeCampoBusca[100];
        char valorBusca[200];
        char nomeCampoAtualiza[100];
        char valorAtualizacao[200];

        // Ler linha de entrada
        if (fgets(entrada, sizeof(entrada), stdin) == NULL) {
            printf("Falha no processamento do arquivo.\n");
            exit(0);
        }

        entrada[strcspn(entrada, "\r\n")] = '\0';

        if (strlen(entrada) == 0) {
            i--;
            continue;
        }

        // Pular número da linha (ex: "1 ", "2 ", etc.)
        char *ptr = pularNumeroLinha(entrada);

        // Separar critério de busca e atualização
        char *separador = strchr(ptr, ' ');
        if (separador == NULL) {
            printf("Falha no processamento do arquivo.\n");
            exit(0);
        }

        *separador = '\0';
        char *criterioBusca = ptr;
        char *criterioAtualizacao = separador + 1;

        // Processar critério de busca
        char *separadorBusca = strchr(criterioBusca, '=');
        if (separadorBusca == NULL) {
            printf("Falha no processamento do arquivo.\n");
            exit(0);
        }
        *separadorBusca = '\0';
        strcpy(nomeCampoBusca, criterioBusca);
        strcpy(valorBusca, separadorBusca + 1);

        // Remover aspas do valor de busca se existirem
        removerAspas(valorBusca);

        // Processar critério de atualização
        char *separadorAtualiza = strchr(criterioAtualizacao, '=');
        if (separadorAtualiza == NULL) {
            printf("Falha no processamento do arquivo.\n");
            exit(0);
        }
        *separadorAtualiza = '\0';
        strcpy(nomeCampoAtualiza, criterioAtualizacao);
        strcpy(valorAtualizacao, separadorAtualiza + 1);

        // USAR MÓDULO DE BUSCA MODULARIZADO
        ResultadoBusca *resultado = buscarPessoas(binArquivo, indiceArquivo, nomeCampoBusca, valorBusca);
        
        if (resultado != NULL && resultado->quantidade > 0) {
            FILE *arquivo = fopen(binArquivo, "r+b");
            if (arquivo == NULL) {
                printf("Falha no processamento do arquivo.\n");
                exit(0);
            }

            // Marcar arquivo como inconsistente
            marcarArquivoInconsistente(arquivo, 0); // 0 = Pessoa

            // Atualizar cada registro encontrado
            for (int j = 0; j < resultado->quantidade; j++) {
                RegistroPessoa pessoaOriginal, pessoaAtualizada;
                
                // Ler registro original
                if (!lerRegistroEmPosicao(arquivo, resultado->posicoes[j], &pessoaOriginal)) {
                    continue;
                }

                // Criar cópia para atualização
                copiarRegistroPessoa(&pessoaAtualizada, &pessoaOriginal);

                // Aplicar atualizações
                int flagID = 0; //usada para verificar se houve tentativa de alterar o ID
                flagID = aplicarAtualizacoes(&pessoaAtualizada, nomeCampoAtualiza, valorAtualizacao);


                if (flagID) {

                    // Carregar índice em memória
                    int tamanhoIndice;
                    RegistroIndice *registrosIndice = CarregarIndiceEmMemoria(indiceArquivo, &tamanhoIndice);
                    //remover do índice em memoria o idPessoa antigo
                    RemoverDoIndice(&registrosIndice, &tamanhoIndice, pessoaOriginal.idPessoa);
                    // Reescrever índice
                    ReescreverIndice(indiceArquivo, registrosIndice, tamanhoIndice);
                    //atualizar o índice com o novo idPessoa
                    atualizarIndice(indiceArquivo, pessoaAtualizada.idPessoa, resultado->posicoes[j]);
                    free(registrosIndice);


                }

                // Calcular tamanhos
                int tamanhoOriginal = pessoaOriginal.tamanhoRegistro;
                int tamanhoAtualizado = calcularTamanhoRegistro(&pessoaAtualizada);

                if (tamanhoAtualizado <= tamanhoOriginal) {
                    // Atualização in-place
                    pessoaAtualizada.tamanhoRegistro = tamanhoOriginal;
                    escreverRegistroEmPosicao(arquivo, resultado->posicoes[j], &pessoaAtualizada);
                    
                    // Preencher espaço restante com lixo
                    if (tamanhoAtualizado < tamanhoOriginal) {
                        preencherComLixo(arquivo, resultado->posicoes[j] + 5 + tamanhoAtualizado, tamanhoOriginal - tamanhoAtualizado);
                    }
                } else {
                    // Remoção lógica + inserção
                    // Marcar como removido
                    marcarRegistroRemovido(arquivo, resultado->posicoes[j]);

                    // Inserir novo registro no final
                    fseek(arquivo, 0, SEEK_END);
                    long long novaPosicao = ftell(arquivo);
                    pessoaAtualizada.tamanhoRegistro = 0; // Será calculado na função de escrita
                    escreverRegistroPessoa(arquivo, &pessoaAtualizada);

                    // Atualizar índice
                    atualizarIndice(indiceArquivo, pessoaAtualizada.idPessoa, novaPosicao);

                    //atualizar cabeçalho
                    CabecalhoPessoa cabecalho;
                    
                    fseek(arquivo, 0, SEEK_SET);
                    // Ler cabeçalho
                    if (!lerCabecalhoPessoa(arquivo, &cabecalho)) {
                        printf("Falha no processamento do arquivo.\n");
                        fclose(arquivo);
                        exit(0);
                 }

                    cabecalho.quantidadeRemovidos++;
                    fseek(arquivo, 0, SEEK_END);
                    cabecalho.proxByteOffset = ftell(arquivo);
                    cabecalho.status = '1';

                    fseek(arquivo, 0, SEEK_SET);
                    escreverCabecalhoPessoa(arquivo, &cabecalho);

                }

                // Liberar memória
                liberarRegistroPessoa(&pessoaOriginal);
                liberarRegistroPessoa(&pessoaAtualizada);
            }

            // Marcar arquivo como consistente
            CabecalhoPessoa cabecalho;
            fseek(arquivo, 0, SEEK_SET);
            lerCabecalhoPessoa(arquivo, &cabecalho);
            cabecalho.status = '1';
            fseek(arquivo, 0, SEEK_SET);
            escreverCabecalhoPessoa(arquivo, &cabecalho);

            fclose(arquivo);
        }
        
        // Liberar memória do resultado
        if (resultado != NULL) {
            liberarResultadoBusca(resultado);
        }
    }

    // Mostrar arquivos binários na tela
    binarioNaTela(binArquivo);
    binarioNaTela(indiceArquivo);
}

