#include "estruturas.h"

// ordena idPessoa para o indice
int compararIdPessoa(const void *a, const void *b) {
    const RegistroIndice *regA = (const RegistroIndice *)a;
    const RegistroIndice *regB = (const RegistroIndice *)b;

    if (regA->idPessoa < regB->idPessoa) return -1;
    if (regA->idPessoa > regB->idPessoa) return 1;
    return 0;
}

// função dada pela professora para imprimir o binário na tela
void binarioNaTela(char *nomeArquivoBinario) {

	/* Use essa função para comparação no run.codes. Lembre-se de ter fechado (fclose) o arquivo anteriormente.
	*  Ela vai abrir de novo para leitura e depois fechar (você não vai perder pontos por isso se usar ela). */

	unsigned long i, cs;
	unsigned char *mb;
	size_t fl;
	FILE *fs;
	if(nomeArquivoBinario == NULL || !(fs = fopen(nomeArquivoBinario, "rb"))) {
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
	printf("%lf\n", (cs / (double) 100));
	free(mb);
	fclose(fs);
}

// função dada pela professora para ler um campo string delimitado entre aspas (")
void scan_quote_string(char *str) {

	/*
	*	Use essa função para ler um campo string delimitado entre aspas (").
	*	Chame ela na hora que for ler tal campo. Por exemplo:
	*
	*	A entrada está da seguinte forma:
	*		nomeDoCampo "MARIA DA SILVA"
	*
	*	Para ler isso para as strings já alocadas str1 e str2 do seu programa, você faz:
	*		scanf("%s", str1); // Vai salvar nomeDoCampo em str1
	*		scan_quote_string(str2); // Vai salvar MARIA DA SILVA em str2 (sem as aspas)
	*
	*/

	char R;

	while((R = getchar()) != EOF && isspace(R)); // ignorar espaços, \r, \n...

	if(R == 'N' || R == 'n') { // campo NULO
		getchar(); getchar(); getchar(); // ignorar o "ULO" de NULO.
		strcpy(str, ""); // copia string vazia
	} else if(R == '\"') {
		if(scanf("%[^\"]", str) != 1) { // ler até o fechamento das aspas
			strcpy(str, "");
		}
		getchar(); // ignorar aspas fechando
	} else if(R != EOF){ // vc tá tentando ler uma string que não tá entre aspas! Fazer leitura normal %s então, pois deve ser algum inteiro ou algo assim...
		str[0] = R;
		scanf("%s", &str[1]);
	} else { // EOF
		strcpy(str, "");
	}
}

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

// função 01 
void CREAT_INDEX(char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "wb");

    if (arquivo == NULL) { 
        printf("Falha no processamento do arquivo.\n");
        exit (0);
    }

    CabecalhoIndice cabecalho;
    cabecalho.status = '0';  // status aberto

    // enche lixo com '$'
    for (int i = 0; i < 11; i++) {
        cabecalho.lixo[i] = '$';
    }

    fwrite(&cabecalho.status, 1 , 1, arquivo);
    fwrite(&cabecalho.lixo,11,1,arquivo);


        // fechaa arquivo e marcar status como fechado
    cabecalho.status = '1';
    fseek(arquivo, 0, SEEK_SET);
    fwrite(&cabecalho.status, sizeof(char), 1, arquivo);

    fclose(arquivo);

    // Mostrar arquivo na tela usando binarioNaTela
    binarioNaTela(nomeArquivo);
}




// calcula o tamanho do registro
int calcularTamanhoRegistro(RegistroPessoa *pessoa) {
    return 4 + 4 + 4 + pessoa->tamanhoNomePessoa + 4 + pessoa->tamanhoNomeUsuario;
}

// escreve um registro pessoa no arquivo
void escreverRegistroPessoa(FILE *arquivo, RegistroPessoa *pessoa) {
    pessoa->removido = '0'; //
    pessoa->tamanhoRegistro = calcularTamanhoRegistro(pessoa);

    fwrite(&pessoa->removido,   sizeof(char), 1, arquivo);
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

// função 02
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

        cabecalhoIndice.status = '0';
        for (int i = 0; i < 11; i++) {
            cabecalhoIndice.lixo[i] = '$';
        }
        fwrite(&cabecalhoIndice.status, 1 , 1, indiceFile);
        fwrite(&cabecalhoIndice.lixo, 11 , 1, indiceFile);

    } else {
        fread(&cabecalhoIndice.status, 1 , 1, indiceFile);
        fread(&cabecalhoIndice.lixo, 11 , 1, indiceFile);
        cabecalhoIndice.status = '0';
        fseek(indiceFile, 0, SEEK_SET);
        fwrite(&cabecalhoIndice.status, sizeof(char), 1, indiceFile);
        fseek(indiceFile, 12, SEEK_SET);
    }

    cabecalhoPessoa.status = '0';
    cabecalhoPessoa.quantidadePessoas = 0;
    cabecalhoPessoa.quantidadeRemovidos = 0;
    cabecalhoPessoa.proxByteOffset = 17; // Tamanho do cabeçalho

    fwrite(&cabecalhoPessoa.status, sizeof(char), 1, binFile);
    fwrite(&cabecalhoPessoa.quantidadePessoas, sizeof(int), 1, binFile);
    fwrite(&cabecalhoPessoa.quantidadeRemovidos, sizeof(int), 1, binFile);
    fwrite(&cabecalhoPessoa.proxByteOffset, sizeof(long long), 1, binFile);

    // pula a primeira linha do CSV
    char linha[1024];
    fgets(linha, sizeof(linha), csv);

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

        if (pessoa.nomePessoa) free(pessoa.nomePessoa);
        if (pessoa.nomeUsuario) free(pessoa.nomeUsuario);
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
    fwrite(&cabecalhoPessoa.status, sizeof(char), 1, binFile);
    fwrite(&cabecalhoPessoa.quantidadePessoas, sizeof(int), 1, binFile);
    fwrite(&cabecalhoPessoa.quantidadeRemovidos, sizeof(int), 1, binFile);
    fwrite(&cabecalhoPessoa.proxByteOffset, sizeof(long long), 1, binFile);

    cabecalhoIndice.status = '1';
    fseek(indiceFile, 0, SEEK_SET);
    fwrite(&cabecalhoIndice.status, sizeof(char), 1, indiceFile);

    fclose(csv);
    fclose(binFile);
    fclose(indiceFile);

    // Mostrar conteúdo dos arquivos
    binarioNaTela(binArquivo);
    //esse ta com problema
    binarioNaTela(indiceArquivo);
}

int lerRegistroPessoa(FILE *arquivo, RegistroPessoa *pessoa) {
    if (fread(&pessoa->removido, sizeof(char), 1, arquivo) != 1) {
        return 0; 
    }

    if (fread(&pessoa->tamanhoRegistro, sizeof(int), 1, arquivo) != 1) {
        return 0;
    }

    if (fread(&pessoa->idPessoa, sizeof(int), 1, arquivo) != 1) return 0;
    if (fread(&pessoa->idadePessoa, sizeof(int), 1, arquivo) != 1) return 0;
    if (fread(&pessoa->tamanhoNomePessoa, sizeof(int), 1, arquivo) != 1) return 0;

    if (pessoa->tamanhoNomePessoa > 0) {
        pessoa->nomePessoa = malloc(pessoa->tamanhoNomePessoa + 1);
        if (pessoa->nomePessoa == NULL) { 
        fprintf(stderr, "Erro: Falha ao alocar memória para nomePessoa.\n");
        printf("ERRO: Falha ao alocar memória para ID %d",pessoa->idPessoa);
        return 0;} 
        if (fread(pessoa->nomePessoa, sizeof(char), pessoa->tamanhoNomePessoa, arquivo) != pessoa->tamanhoNomePessoa) {
            free(pessoa->nomePessoa);
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
        fprintf(stderr, "Erro: Falha ao alocar memória para nomePessoa.\n");
        printf("ERRO: Falha ao alocar memória para ID %d",pessoa->idPessoa);
        return 0;} 

        if (fread(pessoa->nomeUsuario, sizeof(char), pessoa->tamanhoNomeUsuario, arquivo) != pessoa->tamanhoNomeUsuario) {
            if (pessoa->nomePessoa) free(pessoa->nomePessoa);
            free(pessoa->nomeUsuario);
            return 0;
        }
        pessoa->nomeUsuario[pessoa->tamanhoNomeUsuario] = '\0';
    } else {
        pessoa->nomeUsuario = NULL;
    }

    return 1; 
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


    if (fread(&cabecalho.status, sizeof(char), 1, arquivo) != 1) {
        printf("Falha no processamento do arquivo.\n");
        fclose(arquivo);
        exit (0);
    }
    if (fread(&cabecalho.quantidadePessoas, sizeof(int), 1, arquivo) != 1) {
        printf("Falha no processamento do arquivo.\n");
        fclose(arquivo);
        exit (0);
    }
    if (fread(&cabecalho.quantidadeRemovidos, sizeof(int), 1, arquivo) != 1) {
        printf("Falha no processamento do arquivo.\n");
        fclose(arquivo);
        exit (0);
    }
    if (fread(&cabecalho.proxByteOffset, sizeof(long long), 1, arquivo) != 1) {
        printf("Falha no processamento do arquivo.\n");
        fclose(arquivo);
        exit (0);
    }
    // confere se o arquivo está consistente
    if (cabecalho.status != '1') {
        printf("Falha no processamento do arquivo.\n");
        fclose(arquivo);
        exit (0);
    }

    // Percorrer todos os registros sequencialmente
    while (lerRegistroPessoa(arquivo, &pessoa)) {
        if (pessoa.removido == '0') {
            registrosEncontrados++;
            printf("Dados da pessoa de codigo %d\n", pessoa.idPessoa);

            // Nome (ou "-" se nulo)
            printf("Nome: %s\n", pessoa.nomePessoa ? pessoa.nomePessoa : "-");

            // Idade (ou "-" se nulo/invalido)
            if (pessoa.idadePessoa == -1) {
                printf("Idade: -\n");
            } else {
                printf("Idade: %d\n", pessoa.idadePessoa);
            }

            // Usuario (ou "-" se nulo)
            printf("Usuario: %s\n", pessoa.nomeUsuario ? pessoa.nomeUsuario : "-");

            // Linha em branco entre registros
            printf("\n");
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

        // Se nãoo encontrou nenhum registro valido
    if (registrosEncontrados == 0) {
        printf("Registro inexistente.\n");
    }

    fclose(arquivo);
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

// função 04
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
        if (valorBusca[0] == '"' && valorBusca[strlen(valorBusca) - 1] == '"') {
            valorBusca[strlen(valorBusca) - 1] = '\0';
            memmove(valorBusca, valorBusca + 1, strlen(valorBusca));
        }

        int encontrados = 0;

        if (strcmp(nomeCampo, "idPessoa") == 0) {
            int idProcurado = atoi(valorBusca);
            encontrados = buscaSequencial(binArquivo, "idPessoa", valorBusca);
        } else {
            encontrados = buscaSequencial(binArquivo, nomeCampo, valorBusca);
        }

        totalEncontrados += encontrados;
        if (encontrados == 0) printf("Registro inexistente.\n\n");
    }

}





