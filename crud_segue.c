#include "crud_segue.h"
#include "io_arquivos.h"
#include "registro_segue.h"
#include "csv_parser.h"
#include "busca.h"
#include "utils.h"
#include "registro_pessoa.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define debugk 0

// Função 8 - CREATE_TABLE_SEGUE
void CREAT_TABLE_SEGUE(char *csvArquivo, char *binArquivo) {
    FILE *csv, *binFile;
    CabecalhoSegue cabecalhoSegue;
    RegistroSegue segue;
    int quantidadeSegue = 0;

    csv = fopen(csvArquivo, "r");
    if (csv == NULL) {
        printf("Falha no processamento do arquivo.\n");
        exit(0);
    }

    binFile = fopen(binArquivo, "wb");
    if (binFile == NULL) {
        printf("Falha no processamento do arquivo.\n");
        fclose(csv);
        exit(0);
    }

    inicializarCabecalhoSegue(&cabecalhoSegue);
    escreverCabecalhoSegue(binFile, &cabecalhoSegue);

    // pula a primeira linha do CSV (cabeçalho)
    pularCabecalhoCSV(csv);

    // ler cada linha do CSV
    while (ler_csv_segue(csv, &segue)) {
        escreverRegistroSegue(binFile, &segue);
        quantidadeSegue++;
    }

    // Atualizar cabeçalho do arquivo segue
    cabecalhoSegue.status = '1';
    cabecalhoSegue.quantidadeSegue = quantidadeSegue;
    cabecalhoSegue.proxRRN = (ftell(binFile) - 9)/30; // Cada registro ocupa 30 bytes

    fseek(binFile, 0, SEEK_SET);
    escreverCabecalhoSegue(binFile, &cabecalhoSegue);

    fclose(csv);
    fclose(binFile);

    // Mostrar conteúdo do arquivo
    binarioNaTela(binArquivo);
}

// Função 9 - ORDER BY
void ORDER_BY(char *arquivoDesordenado, char *arquivoOrdenado) {
    FILE *arquivoEntrada, *arquivoSaida;
    CabecalhoSegue cabecalho;
    RegistroSegue *registros = NULL;
    int capacidade = 1000;
    int quantidadeRegistros = 0;
    int i;

    // Abrir arquivo de entrada
    arquivoEntrada = fopen(arquivoDesordenado, "rb");
    if (arquivoEntrada == NULL) {
        printf("Falha no processamento do arquivo.\n");
        exit(0);
    }

    // Ler cabeçalho
    if (!lerCabecalhoSegue(arquivoEntrada, &cabecalho)) {
        printf("Falha no processamento do arquivo.\n");
        fclose(arquivoEntrada);
        exit(0);
    }

    // Verificar se arquivo está consistente
    if (!validarArquivoConsistente(cabecalho.status)) {
        printf("Falha no processamento do arquivo.\n");
        fclose(arquivoEntrada);
        exit(0);
    }

    // Alocar memória para os registros
    registros = malloc(capacidade * sizeof(RegistroSegue));
    if (registros == NULL) {
        printf("Falha no processamento do arquivo.\n");
        fclose(arquivoEntrada);
        exit(0);
    }

    // Ler todos os registros do arquivo
    while (lerRegistroSegue(arquivoEntrada, &registros[quantidadeRegistros])) {
        quantidadeRegistros++;
        
        // Redimensionar array se necessário
        if (quantidadeRegistros >= capacidade) {
            capacidade *= 2;
            registros = realloc(registros, capacidade * sizeof(RegistroSegue));
            if (registros == NULL) {
                printf("Falha no processamento do arquivo.\n");
                fclose(arquivoEntrada);
                exit(0);
            }
        }
    }

    fclose(arquivoEntrada);

    // Ordenar registros usando qsort
    qsort(registros, quantidadeRegistros, sizeof(RegistroSegue), compararRegistrosSegue);

    // Criar arquivo de saída
    arquivoSaida = fopen(arquivoOrdenado, "wb");
    if (arquivoSaida == NULL) {
        printf("Falha no processamento do arquivo.\n");
        free(registros);
        exit(0);
    }

    // Escrever cabeçalho
    cabecalho.status = '0';
    cabecalho.quantidadeSegue = quantidadeRegistros;
    cabecalho.proxRRN = 0; 

    escreverCabecalhoSegue(arquivoSaida, &cabecalho);

    // Escrever todos os registros ordenados
    for (i = 0; i < quantidadeRegistros; i++) {
        escreverRegistroSegue(arquivoSaida, &registros[i]);
    }

    // Atualizar cabeçalho com status consistente
    cabecalho.status = '1';
    cabecalho.proxRRN = (ftell(arquivoSaida)-9)/30; // Cada registro ocupa 30 bytes

    fseek(arquivoSaida, 0, SEEK_SET);
    escreverCabecalhoSegue(arquivoSaida, &cabecalho);

    fclose(arquivoSaida);
    free(registros);

    // Mostrar arquivo ordenado na tela
    binarioNaTela(arquivoOrdenado);
}

// Função 10 - JOIN (conforme orientações)
void JOIN(char *arquivoPessoa, char *arquivoIndice, char *arquivoSegue, int n) {
    // Carregar arquivo segue para RAM conforme orientações
    int tamanho;
    RegistroSegue **registrosSegue = LerRegistrosSegue(arquivoSegue, &tamanho);
    
    if (registrosSegue == NULL) {
        printf("Falha no processamento do arquivo.\n");
        exit(0);
    }
    
    for (int i = 0; i < n; i++) {
        char entrada[300];
        char nomeCampo[100];
        char valorBusca[200];
        
        if (fgets(entrada, sizeof(entrada), stdin) == NULL) {
            printf("Falha no processamento do arquivo.\n");
            LiberarRegistrosSegue(registrosSegue, tamanho);
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
            LiberarRegistrosSegue(registrosSegue, tamanho);
            exit(0);
        }
        
        // Separar nomeCampo e valorCampo
        *separador = '\0';
        char *nomeCampoPtr = ptr;
        
        strcpy(nomeCampo, nomeCampoPtr);
        strcpy(valorBusca, separador + 1);
        
        // Se tiver entre aspas, remover as aspas
        removerAspas(valorBusca);
        
        // USAR MÓDULO DE BUSCA MODULARIZADO
        ResultadoBusca *resultado = buscarPessoas(arquivoPessoa, arquivoIndice, nomeCampo, valorBusca);
        
        if (resultado == NULL || resultado->quantidade == 0) {
            printf("Registro inexistente.\n");
            if (resultado != NULL) {
                liberarResultadoBusca(resultado);
            }
            continue;
        }
        
        // Processa todos os resultados encontrados
        for (int j = 0; j < resultado->quantidade; j++) {
            RegistroPessoa pessoa = resultado->pessoas[j];
            
            // Exibir dados da pessoa
            printf("Dados da pessoa de codigo %d\n", pessoa.idPessoa);
            printf("Nome: %s\n", pessoa.nomePessoa ? pessoa.nomePessoa : "-");
            if (pessoa.idadePessoa == -1) {
                printf("Idade: -\n");
            } else {
                printf("Idade: %d\n", pessoa.idadePessoa);
            }
            printf("Usuario: %s\n", pessoa.nomeUsuario ? pessoa.nomeUsuario : "-");
            printf("\n");
            
            // Buscar registros no array segue carregado na RAM conforme orientações
            int idPessoa = pessoa.idPessoa;
            
            // Achar a posição de um item nos registros segue usando busca binária
            int posicaoAchada = BuscaBinariaSegue(registrosSegue, tamanho, idPessoa);
            
            if (posicaoAchada == -1) {
                // Não encontrou nenhum registro
                printf("Registro inexistente.\n");
            } else {
                // Achar todos os items para trás
                int k = posicaoAchada;
                while (k >= 0 && registrosSegue[k]->idPessoaQueSegue == idPessoa) {
                    k--;
                }

                k++; // Voltar para o primeiro item válido
                while(k <= posicaoAchada && registrosSegue[k]->idPessoaQueSegue == idPessoa) {
                    // Exibe a pessoa e o segue atual
                    printf("Segue a pessoa de codigo: %d\n", registrosSegue[k]->idPessoaQueESeguida);
                    
                    // Justificativa para seguir
                    if (registrosSegue[k]->grauAmizade == '0') {
                        printf("Justificativa para seguir: celebridade\n");
                    } else if (registrosSegue[k]->grauAmizade == '1') {
                        printf("Justificativa para seguir: amiga de minha amiga\n");
                    } else if (registrosSegue[k]->grauAmizade == '2') {
                        printf("Justificativa para seguir: minha amiga\n");
                    } else {
                        printf("Justificativa para seguir: -\n");
                    }
                    
                    // Imprimir data início
                    printf("Começou a seguir em: ");
                
                    int datainvalida = 0;
                    for (int l = 0; l < 10; l++) {
                        if (registrosSegue[k]->dataInicioQueSegue[l] == '$') {
                            datainvalida = 1;
                            break;
                        }
                    }
                
                    if (datainvalida) {
                        printf("-");
                    } else {
                        for (int l = 0; l < 10; l++) {
                            printf("%c", registrosSegue[k]->dataInicioQueSegue[l]);
                        }
                    }
                    printf("\n");

                    // Imprimir data fim
                    printf("Parou de seguir em: ");

                    datainvalida = 0;
                    for (int l = 0; l < 10; l++) {
                        if (registrosSegue[k]->dataFimQueSegue[l] == '$') {
                            datainvalida = 1;
                            break;
                        }
                    }

                    if (datainvalida) {
                        printf("-");
                    } else {
                        for (int l = 0; l < 10; l++) {
                            printf("%c", registrosSegue[k]->dataFimQueSegue[l]);
                        }
                    }
                    printf("\n\n");
                    k++;
                }
                
                // Achar todos para frente
                k = posicaoAchada + 1;
                while (k < tamanho && registrosSegue[k]->idPessoaQueSegue == idPessoa) {
                    // Exibe a pessoa e o segue atual
                    printf("Segue a pessoa de codigo: %d\n", registrosSegue[k]->idPessoaQueESeguida);
                    
                    // Justificativa para seguir
                    if (registrosSegue[k]->grauAmizade == '0') {
                        printf("Justificativa para seguir: celebridade\n");
                    } else if (registrosSegue[k]->grauAmizade == '1') {
                        printf("Justificativa para seguir: amiga de minha amiga\n");
                    } else if (registrosSegue[k]->grauAmizade == '2') {
                        printf("Justificativa para seguir: minha amiga\n");
                    } else {
                        printf("Justificativa para seguir: -\n");
                    }
                    
                    // Imprimir data início
                    printf("Começou a seguir em: ");
                
                    int datainvalida = 0;
                    for (int l = 0; l < 10; l++) {
                        if (registrosSegue[k]->dataInicioQueSegue[l] == '$') {
                            datainvalida = 1;
                            break;
                        }
                    }
                
                    if (datainvalida) {
                        printf("-");
                    } else {
                        for (int l = 0; l < 10; l++) {
                            printf("%c", registrosSegue[k]->dataInicioQueSegue[l]);
                        }
                    }
                    printf("\n");

                    // Imprimir data fim
                    printf("Parou de seguir em: ");

                    datainvalida = 0;
                    for (int l = 0; l < 10; l++) {
                        if (registrosSegue[k]->dataFimQueSegue[l] == '$') {
                            datainvalida = 1;
                            break;
                        }
                    }

                    if (datainvalida) {
                        printf("-");
                    } else {
                        for (int l = 0; l < 10; l++) {
                            printf("%c", registrosSegue[k]->dataFimQueSegue[l]);
                        }
                    }
                    printf("\n\n");
                    k++;
                }
            }

            printf("\n");
        }
        
        // Liberar memória do resultado de busca
        if (resultado != NULL) {
            liberarResultadoBusca(resultado);
        }
    }
    
    // Liberar memória dos registros segue carregados na RAM
    LiberarRegistrosSegue(registrosSegue, tamanho);
}

