#include "estruturas.h"
#include "busca.h"

#define debug 1

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

        // USAR MÓDULO DE BUSCA MODULARIZADO
        ResultadoBusca *resultado = buscarPessoas(binArquivo, indiceArquivo, nomeCampo, valorBusca);
        int encontrados = 0;
        
        if (resultado != NULL && resultado->quantidade > 0) {
            for (int j = 0; j < resultado->quantidade; j++) {
                printf("Dados da pessoa de codigo %d\n", resultado->pessoas[j].idPessoa);
                printf("Nome: %s\n", resultado->pessoas[j].nomePessoa ? resultado->pessoas[j].nomePessoa : "-");
                if (resultado->pessoas[j].idadePessoa == -1) {
                    printf("Idade: -\n");
                } else {
                    printf("Idade: %d\n", resultado->pessoas[j].idadePessoa);
                }
                printf("Usuario: %s\n", resultado->pessoas[j].nomeUsuario ? resultado->pessoas[j].nomeUsuario : "-");
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


/*
----------------------------------------- COMEÇANDO PARTE 2 -------------------------------------------------------------
*/

// Função 5 - DELETE FROM


// Função para carregar índice em memória
RegistroIndice* CarregarIndiceEmMemoria(char *nomeArquivo, int *tamanho) {
    
    if(debug) printf("carregando indice em memoria...\n");
    
    FILE *arquivo;
    CabecalhoIndice cabecalho;
    RegistroIndice *registros = NULL;
    int capacidade = 1000;
    int quantidadeRegistros = 0;
     


    arquivo = fopen(nomeArquivo, "rb");

    if (arquivo == NULL) {
        printf("Falha no processamento do arquivo.\n");
        exit(0);
    }
    
    // Ler cabeçalho
    if (fread(&cabecalho.status, sizeof(char), 1, arquivo) != 1 ||
        fread(&cabecalho.lixo, 11, 1, arquivo) != 1) {
        fclose(arquivo);
        if(debug) printf("erro ao ler o cabeçalho\n");
        return NULL;
    }
    
    if (cabecalho.status != '1') {
        fclose(arquivo);
        if(debug) printf("arquivo inconsistente\n");
        return NULL;
    }
    
    // Calcular quantidade de registros no índice
    fseek(arquivo, 0, SEEK_END);
    long long tamanhoArquivo = ftell(arquivo);
    int totalRegistros = (tamanhoArquivo - 12) / 12; // 12 bytes por registro (4 + 8)
    
    if (totalRegistros <= 0) {
        fclose(arquivo);
        *tamanho = 0;
        if(debug) printf("nenhum registro no indice\n");
        return NULL;
    }
    
    // Alocar memória para os registros
    registros = malloc(totalRegistros * sizeof(RegistroIndice));
    if (registros == NULL) {
        fclose(arquivo);
        if(debug) printf("falha ao alocar memoria para os registros\n");
        return NULL;
    }
    
    // Ler todos os registros do índice
    fseek(arquivo, 12, SEEK_SET);
    for (int i = 0; i < totalRegistros; i++) {
        if (fread(&registros[i].idPessoa, sizeof(int), 1, arquivo) != 1 ||
            fread(&registros[i].byteOffset, sizeof(long long), 1, arquivo) != 1) {
            free(registros);
            fclose(arquivo);
            if(debug) printf("erro ao ler os registros do indice\n");
            return NULL;

        }
        quantidadeRegistros++;
    }
    
    fclose(arquivo);
    *tamanho = quantidadeRegistros;
    if(debug) printf("indice carregado em memoria com %d registros\n", quantidadeRegistros);
    return registros;
}

// Função para remover registro do índice em memória
void RemoverDoIndice(RegistroIndice **registros, int *tamanho, int idPessoa) {
    for (int i = 0; i < *tamanho; i++) {
        if ((*registros)[i].idPessoa == idPessoa) {
            // Mover todos os registros uma posição para a esquerda
            for (int j = i; j < *tamanho - 1; j++) {
                (*registros)[j] = (*registros)[j + 1];
            }
            (*tamanho)--;
            break;
        }
    }
}

// Função para reescrever arquivo de índice
void ReescreverIndice(char *nomeArquivo, RegistroIndice *registros, int tamanho) {
    FILE *arquivo;
    CabecalhoIndice cabecalho;
    
    // Abrir arquivo no modo escrita binária (apaga conteúdo)
    arquivo = fopen(nomeArquivo, "wb");
    if (arquivo == NULL) {
        return;
    }
    
    // Escrever cabeçalho
    cabecalho.status = '0';
    for (int i = 0; i < 11; i++) {
        cabecalho.lixo[i] = '$';
    }
    
    fwrite(&cabecalho.status, sizeof(char), 1, arquivo);
    fwrite(&cabecalho.lixo, 11, 1, arquivo);
    
    // Escrever todos os registros
    for (int i = 0; i < tamanho; i++) {
        fwrite(&registros[i].idPessoa, sizeof(int), 1, arquivo);
        fwrite(&registros[i].byteOffset, sizeof(long long), 1, arquivo);
    }
    
    // Marcar arquivo como consistente
    cabecalho.status = '1';
    fseek(arquivo, 0, SEEK_SET);
    fwrite(&cabecalho.status, sizeof(char), 1, arquivo);
    
    fclose(arquivo);
}

// Função 5 - DELETE FROM
void DELETE_FROM(char *binArquivo, char *indiceArquivo) {
    int n;
    scanf("%d", &n);
    
    // Carregar índice em memória
    int tamanhoIndice;

    if(debug) printf("Carregando indice em memoria para DELETE...\n");

    RegistroIndice *registrosIndice = CarregarIndiceEmMemoria(indiceArquivo, &tamanhoIndice);
    
    if(debug) printf("Indice carregado com %d registros\n", tamanhoIndice);

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
        char *ptr = entrada;
        while (*ptr && (isdigit(*ptr) || isspace(*ptr))) {
            ptr++;
            if(debug) printf("pulando numero da linha: %c\n", *(ptr-1));
        }
        
        // Procura símbolo = para separar campo e valor
        char *separador = strchr(ptr, '=');
        if (separador == NULL) {
            printf("Falha no processamento do arquivo.\n");
            if(debug) printf("Erro ao encontrar o separador '=' na entrada: %s\n", entrada);
            free(registrosIndice);
            exit(0);
        }
        
        // Separar nomeCampo e valorCampo
        *separador = '\0';
        char *nomeCampoPtr = ptr;
        
        // Pular números e espaços no início
        while (*nomeCampoPtr && (isdigit(*nomeCampoPtr) || isspace(*nomeCampoPtr))) {
            nomeCampoPtr++;
        }
        
        strcpy(nomeCampo, nomeCampoPtr);
        strcpy(valorBusca, separador + 1);
        if(debug) printf("Campo: %s, Valor: %s\n", nomeCampo, valorBusca);
        
        // Se tiver entre aspas, remover as aspas
        if (valorBusca[0] == '"' && valorBusca[strlen(valorBusca) - 1] == '"') {
            valorBusca[strlen(valorBusca) - 1] = '\0';
            memmove(valorBusca, valorBusca + 1, strlen(valorBusca));
        }
        
        // USAR MÓDULO DE BUSCA MODULARIZADO
        ResultadoBusca *resultado = buscarPessoas(binArquivo, indiceArquivo, nomeCampo, valorBusca);
        
        if (resultado != NULL && resultado->quantidade > 0) {
            // Marcar registros como removidos no arquivo de dados
            if (arquivo != NULL) {
                CabecalhoPessoa cabecalho;
                fread(&cabecalho.status, sizeof(char), 1, arquivo);
                fread(&cabecalho.quantidadePessoas, sizeof(int), 1, arquivo);
                fread(&cabecalho.quantidadeRemovidos, sizeof(int), 1, arquivo);
                fread(&cabecalho.proxByteOffset, sizeof(long long), 1, arquivo);
                
                // Marcar arquivo como inconsistente
                cabecalho.status = '0';
                fseek(arquivo, 0, SEEK_SET);
                fwrite(&cabecalho.status, sizeof(char), 1, arquivo);
                
                // Marcar cada registro como removido
                for (int j = 0; j < resultado->quantidade; j++) {
                    fseek(arquivo, resultado->posicoes[j], SEEK_SET);
                    char removido = '1';
                    fwrite(&removido, sizeof(char), 1, arquivo);
                    cabecalho.quantidadeRemovidos++;
                    
                    // Remover do índice em memória
                    // Encontrar o ID da pessoa removida
                    for (int k = 0; k < tamanhoIndice; k++) {
                        if (registrosIndice[k].byteOffset == resultado->posicoes[j]) {
                            RemoverDoIndice(&registrosIndice, &tamanhoIndice, registrosIndice[k].idPessoa);
                            break;
                        }
                    }
                }
                
                // Marcar arquivo como consistente
                cabecalho.status = '1';
                fseek(arquivo, 0, SEEK_SET);
                fwrite(&cabecalho.status, sizeof(char), 1, arquivo);
                fwrite(&cabecalho.quantidadePessoas, sizeof(int), 1, arquivo);
                fwrite(&cabecalho.quantidadeRemovidos, sizeof(int), 1, arquivo);
                fwrite(&cabecalho.proxByteOffset, sizeof(long long), 1, arquivo);
                
                
            }
            else {
                printf("Falha no processamento do arquivo.\n");
                exit(0);
            }
        }
        
        // Liberar memória do resultado
        if (resultado != NULL) {
            liberarResultadoBusca(resultado);
        }
    } fclose(arquivo);
    
    if(debug) binarioNaTela(indiceArquivo);

    // Reescrever arquivo de índice com registros não removidos
    ReescreverIndice(indiceArquivo, registrosIndice, tamanhoIndice);
    
    // Liberar memória do índice
    free(registrosIndice);
    
    // Mostrar arquivos binários na tela
    binarioNaTela(binArquivo);
    binarioNaTela(indiceArquivo);
}

// Função auxiliar para verificar se ID já existe
int verificarIdExistente(char *binArquivo, int idPessoa) {
    FILE *arquivo;
    CabecalhoPessoa cabecalho;
    RegistroPessoa pessoa;

    arquivo = fopen(binArquivo, "rb");
    if (arquivo == NULL) {
        return 0; // Se não conseguir abrir, assume que não existe
    }

    // Ler cabeçalho
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

    // Percorrer todos os registros
    while (lerRegistroPessoa(arquivo, &pessoa)) {
        if (pessoa.removido == '0' && pessoa.idPessoa == idPessoa) {
            // Liberar memória
            if (pessoa.nomePessoa) {
                free(pessoa.nomePessoa);
                pessoa.nomePessoa = NULL;
            }
            if (pessoa.nomeUsuario) {
                free(pessoa.nomeUsuario);
                pessoa.nomeUsuario = NULL;
            }
            fclose(arquivo);
            return 1; // ID já existe
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
    return 0; // ID não existe
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

    // Marcar arquivo como inconsistente durante modificação
    cabecalho.status = '0';
    fseek(arquivo, 0, SEEK_SET);
    fwrite(&cabecalho.status, sizeof(char), 1, arquivo);

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
    fwrite(&cabecalho.status, sizeof(char), 1, arquivo);
    fwrite(&cabecalho.quantidadePessoas, sizeof(int), 1, arquivo);
    fwrite(&cabecalho.quantidadeRemovidos, sizeof(int), 1, arquivo);
    fwrite(&cabecalho.proxByteOffset, sizeof(long long), 1, arquivo);

    fclose(arquivo);
    return posicaoInsercao;
}

// Função auxiliar para atualizar índice
void atualizarIndice(char *indiceArquivo, int idPessoa, long long byteOffset) {
    FILE *arquivo;
    CabecalhoIndice cabecalho;
    RegistroIndice novoRegistro;
    RegistroIndice *registros;
    int quantidadeRegistros;
    int i, j;

    arquivo = fopen(indiceArquivo, "r+b");
    if (arquivo == NULL) {
        printf("Falha no processamento do arquivo.\n");
        exit(0);
    }

    // Ler cabeçalho
    if (fread(&cabecalho.status, sizeof(char), 1, arquivo) != 1 ||
        fread(&cabecalho.lixo, 11, 1, arquivo) != 1) {
        printf("Falha no processamento do arquivo.\n");
        fclose(arquivo);
        exit(0);
    }

    if (cabecalho.status != '1') {
        printf("Falha no processamento do arquivo.\n");
        fclose(arquivo);
        exit(0);
    }

    // Calcular quantidade de registros no índice
    fseek(arquivo, 0, SEEK_END);
    long long tamanhoArquivo = ftell(arquivo);
    quantidadeRegistros = (tamanhoArquivo - 12) / 12; // 12 bytes por registro (4 + 8)

    // Ler todos os registros do índice
    registros = malloc(quantidadeRegistros * sizeof(RegistroIndice));
    if (registros == NULL) {
        printf("Falha no processamento do arquivo.\n");
        fclose(arquivo);
        exit(0);
    }

    fseek(arquivo, 12, SEEK_SET);
    for (i = 0; i < quantidadeRegistros; i++) {
        fread(&registros[i].idPessoa, sizeof(int), 1, arquivo);
        fread(&registros[i].byteOffset, sizeof(long long), 1, arquivo);
    }

    // Adicionar novo registro
    novoRegistro.idPessoa = idPessoa;
    novoRegistro.byteOffset = byteOffset;

    // Encontrar posição correta para inserir (ordem crescente por ID)
    int posicaoInsercao = quantidadeRegistros;
    for (i = 0; i < quantidadeRegistros; i++) {
        if (registros[i].idPessoa > idPessoa) {
            posicaoInsercao = i;
            break;
        }
    }

    // Deslocar registros para abrir espaço
    for (i = quantidadeRegistros; i > posicaoInsercao; i--) {
        registros[i] = registros[i-1];
    }

    // Inserir novo registro
    registros[posicaoInsercao] = novoRegistro;
    quantidadeRegistros++;

    // Marcar arquivo como inconsistente
    cabecalho.status = '0';
    fseek(arquivo, 0, SEEK_SET);
    fwrite(&cabecalho.status, sizeof(char), 1, arquivo);

    // Reescrever todos os registros
    fseek(arquivo, 12, SEEK_SET);
    for (i = 0; i < quantidadeRegistros; i++) {
        fwrite(&registros[i].idPessoa, sizeof(int), 1, arquivo);
        fwrite(&registros[i].byteOffset, sizeof(long long), 1, arquivo);
    }

    // Marcar arquivo como consistente
    cabecalho.status = '1';
    fseek(arquivo, 0, SEEK_SET);
    fwrite(&cabecalho.status, sizeof(char), 1, arquivo);

    free(registros);
    fclose(arquivo);
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
            if(debug) printf("Erro ao ler a linha  %d de entrada para INSERT_INTO\n",i+1);
            exit(0);
        }

        linha[strcspn(linha, "\r\n")] = '\0';

        if (strlen(linha) == 0) {
            i--;
            continue;
        }

        // Pular número da linha (ex: "1 ", "2 ", etc.)
        char *ptr = linha;
        int numerosPulados = 0;
        while (*ptr && ((isdigit(*ptr) && (!numerosPulados)) || isspace(*ptr))) {
            if(isdigit(*ptr)) numerosPulados++;
            ptr++;
        }

        // Separar campos por vírgula
        char *token = strtok(ptr, ",");
        while (token != NULL && numCampos < 4) {
            // Remover espaços em branco do início e fim
            while (*token == ' ') token++;
            char *end = token + strlen(token) - 1;
            while (end > token && *end == ' ') end--;
            *(end + 1) = '\0';

            strcpy(campos[numCampos], token);
            if(debug) printf("Campo %d: %s\n", numCampos + 1, campos[numCampos]);
            numCampos++;
            token = strtok(NULL, ",");
        }

        if (numCampos < 4) {
            printf("Falha no processamento do arquivo.\n");
            if(debug) printf("Erro: número insuficiente de campos (%d) na linha %d para INSERT_INTO\n",numCampos, i + 1);

            exit(0);
        }

        // Processar idPessoa
        pessoa.idPessoa = atoi(campos[0]);

        // Verificar se ID já existe
        if (verificarIdExistente(binArquivo, pessoa.idPessoa)) {
            printf("Falha no processamento do arquivo.\n");
            if(debug) printf("Erro: ID %d já existe no arquivo para INSERT_INTO\n", pessoa.idPessoa);
            exit(0);
        }

        // Processar nomePessoa
        if (strcmp(campos[1], "NULO") == 0) {
            pessoa.tamanhoNomePessoa = 0;
            pessoa.nomePessoa = NULL;
        } else {
            // Remover aspas se existirem
            if (campos[1][0] == '"' && campos[1][strlen(campos[1])-1] == '"') {
                campos[1][strlen(campos[1])-1] = '\0';
                memmove(campos[1], campos[1] + 1, strlen(campos[1]) + 1 );
            }
            pessoa.tamanhoNomePessoa = strlen(campos[1]);
            pessoa.nomePessoa = malloc(pessoa.tamanhoNomePessoa + 1);
            if (pessoa.nomePessoa == NULL) {
                printf("Falha no processamento do arquivo.\n");
                if(debug) printf("Erro: Falha ao alocar memória para nomePessoa no INSERT_INTO\n");
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
            // Remover aspas se existirem
            if (campos[3][0] == '"' && campos[3][strlen(campos[3])-1] == '"') {
                campos[3][strlen(campos[3])-1] = '\0';
                memmove(campos[3], campos[3] + 1, strlen(campos[3]) + 1);
            }
            pessoa.tamanhoNomeUsuario = strlen(campos[3]);
            pessoa.nomeUsuario = malloc(pessoa.tamanhoNomeUsuario + 1);
            if (pessoa.nomeUsuario == NULL) {
                if (pessoa.nomePessoa) free(pessoa.nomePessoa);
                printf("Falha no processamento do arquivo.\n");
                if(debug) printf("Erro: Falha ao alocar memória para nomeUsuario no INSERT_INTO\n");
                exit(0);
            }
            strcpy(pessoa.nomeUsuario, campos[3]);
        }

        // Inserir registro no arquivo
        byteOffset = inserirRegistroPessoa(binArquivo, &pessoa);

        // Atualizar índice
        atualizarIndice(indiceArquivo, pessoa.idPessoa, byteOffset);

        // Liberar memória
        if (pessoa.nomePessoa) {
            free(pessoa.nomePessoa);
        }
        if (pessoa.nomeUsuario) {
            free(pessoa.nomeUsuario);
        }
    }

    // Mostrar arquivos binários na tela
    binarioNaTela(binArquivo);
    binarioNaTela(indiceArquivo);
}


// Função auxiliar para aplicar atualizações em um registro
void aplicarAtualizacoes(RegistroPessoa *pessoa, char *nomeCampo, char *valorAtualizacao) {
    if (strcmp(nomeCampo, "idPessoa") == 0) {
        // Não permitir atualização de idPessoa
        return;
    } else if (strcmp(nomeCampo, "nomePessoa") == 0) {
        if (strcmp(valorAtualizacao, "NULO") == 0) {
            if (pessoa->nomePessoa) {
                free(pessoa->nomePessoa);
            }
            pessoa->nomePessoa = NULL;
            pessoa->tamanhoNomePessoa = 0;
        } else {
            // Remover aspas se existirem
            if (valorAtualizacao[0] == '"' && valorAtualizacao[strlen(valorAtualizacao)-1] == '"') {
                valorAtualizacao[strlen(valorAtualizacao)-1] = '\0';
                memmove(valorAtualizacao, valorAtualizacao + 1, strlen(valorAtualizacao));
            }
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
            // Remover aspas se existirem
            if (valorAtualizacao[0] == '"' && valorAtualizacao[strlen(valorAtualizacao)-1] == '"') {
                valorAtualizacao[strlen(valorAtualizacao)-1] = '\0';
                memmove(valorAtualizacao, valorAtualizacao + 1, strlen(valorAtualizacao));
            }
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
}

// Função auxiliar para ler registro em posição específica
int lerRegistroEmPosicao(FILE *arquivo, long long posicao, RegistroPessoa *pessoa) {
    fseek(arquivo, posicao, SEEK_SET);
    return lerRegistroPessoa(arquivo, pessoa);
}

// Função auxiliar para escrever registro em posição específica
void escreverRegistroEmPosicao(FILE *arquivo, long long posicao, RegistroPessoa *pessoa) {
    fseek(arquivo, posicao, SEEK_SET);
    escreverRegistroPessoa(arquivo, pessoa);
}

// Função auxiliar para preencher com lixo
void preencherComLixo(FILE *arquivo, long long posicao, int tamanho) {
    fseek(arquivo, posicao, SEEK_SET);
    for (int i = 0; i < tamanho; i++) {
        char lixo = '$';
        fwrite(&lixo, sizeof(char), 1, arquivo);
    }
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
        char *ptr = entrada;
        while (*ptr && (isdigit(*ptr) || isspace(*ptr))) {
            ptr++;
        }

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
        if (valorBusca[0] == '"' && valorBusca[strlen(valorBusca) - 1] == '"') {
            valorBusca[strlen(valorBusca) - 1] = '\0';
            memmove(valorBusca, valorBusca + 1, strlen(valorBusca));
        }

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
        int encontrados = 0;
        
        if (resultado != NULL && resultado->quantidade > 0) {
            encontrados = resultado->quantidade;
            FILE *arquivo = fopen(binArquivo, "r+b");
            if (arquivo == NULL) {
                printf("Falha no processamento do arquivo.\n");
                exit(0);
            }

            // Marcar arquivo como inconsistente
            CabecalhoPessoa cabecalho;
            fread(&cabecalho.status, sizeof(char), 1, arquivo);
            fread(&cabecalho.quantidadePessoas, sizeof(int), 1, arquivo);
            fread(&cabecalho.quantidadeRemovidos, sizeof(int), 1, arquivo);
            fread(&cabecalho.proxByteOffset, sizeof(long long), 1, arquivo);

            cabecalho.status = '0';
            fseek(arquivo, 0, SEEK_SET);
            fwrite(&cabecalho.status, sizeof(char), 1, arquivo);

            // Atualizar cada registro encontrado
            for (int j = 0; j < resultado->quantidade; j++) {
                RegistroPessoa pessoaOriginal, pessoaAtualizada;
                
                // Ler registro original
                if (!lerRegistroEmPosicao(arquivo, resultado->posicoes[j], &pessoaOriginal)) {
                    continue;
                }

                // Criar cópia para atualização
                pessoaAtualizada = pessoaOriginal;
                pessoaAtualizada.nomePessoa = NULL;
                pessoaAtualizada.nomeUsuario = NULL;

                // Copiar strings se existirem
                if (pessoaOriginal.nomePessoa) {
                    pessoaAtualizada.tamanhoNomePessoa = pessoaOriginal.tamanhoNomePessoa;
                    pessoaAtualizada.nomePessoa = malloc(pessoaOriginal.tamanhoNomePessoa + 1);
                    if (pessoaAtualizada.nomePessoa) {
                        strcpy(pessoaAtualizada.nomePessoa, pessoaOriginal.nomePessoa);
                    }
                }
                if (pessoaOriginal.nomeUsuario) {
                    pessoaAtualizada.tamanhoNomeUsuario = pessoaOriginal.tamanhoNomeUsuario;
                    pessoaAtualizada.nomeUsuario = malloc(pessoaOriginal.tamanhoNomeUsuario + 1);
                    if (pessoaAtualizada.nomeUsuario) {
                        strcpy(pessoaAtualizada.nomeUsuario, pessoaOriginal.nomeUsuario);
                    }
                }

                // Aplicar atualizações
                aplicarAtualizacoes(&pessoaAtualizada, nomeCampoAtualiza, valorAtualizacao);

                // Calcular tamanhos
                int tamanhoOriginal = pessoaOriginal.tamanhoRegistro;
                int tamanhoAtualizado = calcularTamanhoRegistro(&pessoaAtualizada);

                if (tamanhoAtualizado <= tamanhoOriginal) {
                    // Atualização in-place
                    pessoaAtualizada.tamanhoRegistro = tamanhoAtualizado;
                    escreverRegistroEmPosicao(arquivo, resultado->posicoes[j], &pessoaAtualizada);
                    
                    // Preencher espaço restante com lixo
                    if (tamanhoAtualizado < tamanhoOriginal) {
                        preencherComLixo(arquivo, resultado->posicoes[j] + 5 + tamanhoAtualizado, tamanhoOriginal - tamanhoAtualizado);
                    }
                } else {
                    // Remoção lógica + inserção
                    // Marcar como removido
                    fseek(arquivo, resultado->posicoes[j], SEEK_SET);
                    char removido = '1';
                    fwrite(&removido, sizeof(char), 1, arquivo);

                    // Inserir novo registro no final
                    fseek(arquivo, 0, SEEK_END);
                    long long novaPosicao = ftell(arquivo);
                    pessoaAtualizada.tamanhoRegistro = tamanhoAtualizado;
                    escreverRegistroPessoa(arquivo, &pessoaAtualizada);

                    // Atualizar índice
                    atualizarIndice(indiceArquivo, pessoaAtualizada.idPessoa, novaPosicao);
                }

                // Liberar memória
                if (pessoaOriginal.nomePessoa) free(pessoaOriginal.nomePessoa);
                if (pessoaOriginal.nomeUsuario) free(pessoaOriginal.nomeUsuario);
                if (pessoaAtualizada.nomePessoa) free(pessoaAtualizada.nomePessoa);
                if (pessoaAtualizada.nomeUsuario) free(pessoaAtualizada.nomeUsuario);
            }

            // Marcar arquivo como consistente
            cabecalho.status = '1';
            fseek(arquivo, 0, SEEK_SET);
            fwrite(&cabecalho.status, sizeof(char), 1, arquivo);

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

// Função auxiliar para calcular o tamanho do registro segue
int calcularTamanhoRegistroSegue(RegistroSegue *segue) {
    return 4 + 4 + 4 + 10 + 10 + 4;
}

// Função auxiliar para escrever um registro segue no arquivo
void escreverRegistroSegue(FILE *arquivo, RegistroSegue *segue) {
    segue->removido = '0';
    segue->tamanhoRegistro = calcularTamanhoRegistroSegue(segue);

    fwrite(&segue->removido, sizeof(char), 1, arquivo);
    fwrite(&segue->idPessoaQueSegue, sizeof(int), 1, arquivo);
    fwrite(&segue->idPessoaQueESeguida, sizeof(int), 1, arquivo);
    fwrite(segue->dataInicioQueSegue, sizeof(char), 10, arquivo);
    fwrite(segue->dataFimQueSegue, sizeof(char), 10, arquivo);


    fwrite(&segue->grauAmizade, sizeof(int), 1, arquivo);
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

    if (numCampos > 2 && strlen(campos[2]) > 0) {
        strcpy(segue->dataInicioQueSegue, campos[2]);
    }/* else {
        segue->dataInicioQueSegue = NULL;
    }
      espero que todas as datas sejam datas corretamente, pois nao sei o que é pra colocar se elas nao forem dadas  */
    if (numCampos > 3 && strlen(campos[3]) > 0) {
        strcpy(segue->dataFimQueSegue, campos[3]);
    }/* else {
        segue->dataFimQueSegue = NULL;
    }
        espero que todas as datas sejam datas corretamente, pois nao sei o que é pra colocar se elas nao forem dadas  */
    if (numCampos > 4 && strlen(campos[4]) > 0) {
        segue->grauAmizade = atoi(campos[4]);
    } else {
        segue->grauAmizade = -1; // Valor nulo
    }

    for (int i = 0; i < numCampos; i++) {
        free(campos[i]);
    }

    return 1; 
}

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

    cabecalhoSegue.status = '0';
    cabecalhoSegue.quantidadeSegue = 0;
    cabecalhoSegue.quantidadeRemovidos = 0;
    cabecalhoSegue.proxByteOffset = 17; // Tamanho do cabeçalho

    fwrite(&cabecalhoSegue.status, sizeof(char), 1, binFile);
    fwrite(&cabecalhoSegue.quantidadeSegue, sizeof(int), 1, binFile);
    fwrite(&cabecalhoSegue.quantidadeRemovidos, sizeof(int), 1, binFile);
    fwrite(&cabecalhoSegue.proxByteOffset, sizeof(long long), 1, binFile);

    // pula a primeira linha do CSV (cabeçalho)
    char linha[1024];
    fgets(linha, sizeof(linha), csv);

    // ler cada linha do CSV
    while (ler_csv_segue(csv, &segue)) {
        escreverRegistroSegue(binFile, &segue);
        quantidadeSegue++;

      //  if (segue.dataInicioQueSegue) free(segue.dataInicioQueSegue);
      //  if (segue.dataFimQueSegue) free(segue.dataFimQueSegue);
    }

    // Atualizar cabeçalho do arquivo segue
    cabecalhoSegue.status = '1';
    cabecalhoSegue.quantidadeSegue = quantidadeSegue;
    cabecalhoSegue.proxByteOffset = ftell(binFile);

    fseek(binFile, 0, SEEK_SET);
    fwrite(&cabecalhoSegue.status, sizeof(char), 1, binFile);
    fwrite(&cabecalhoSegue.quantidadeSegue, sizeof(int), 1, binFile);
    fwrite(&cabecalhoSegue.quantidadeRemovidos, sizeof(int), 1, binFile);
    fwrite(&cabecalhoSegue.proxByteOffset, sizeof(long long), 1, binFile);

    fclose(csv);
    fclose(binFile);

    // Mostrar conteúdo do arquivo
    binarioNaTela(binArquivo);
}

// Função auxiliar para ler um registro segue do arquivo binário
int lerRegistroSegue(FILE *arquivo, RegistroSegue *segue) {
    if (fread(&segue->removido, sizeof(char), 1, arquivo) != 1) {
        return 0; 
    }

    
    if (fread(&segue->idPessoaQueSegue, sizeof(int), 1, arquivo) != 1) return 0;
    if (fread(&segue->idPessoaQueESeguida, sizeof(int), 1, arquivo) != 1) return 0;
    if (fread(segue->dataInicioQueSegue, sizeof(char), 10, arquivo) != 10) {
            return 0;
        }
      //  segue->dataInicioQueSegue[segue->tamanhoDataInicio] = '\0';

    // Ler dataFimQueSegue
    if (fread(segue->dataFimQueSegue, sizeof(char), 10, arquivo) != 10) {
            return 0;
        }
       // segue->dataFimQueSegue[segue->tamanhoDataFim] = '\0';
    

    if (fread(&segue->grauAmizade, sizeof(int), 1, arquivo) != 1) {
       // if (segue->dataInicioQueSegue) free(segue->dataInicioQueSegue);
       // if (segue->dataFimQueSegue) free(segue->dataFimQueSegue);
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
    if (regA->dataInicioQueSegue == NULL && regB->dataInicioQueSegue == NULL) {
        // Ambos nulos, continuar para próximo critério
    } else if (regA->dataInicioQueSegue == NULL) {
        return 1; // A é nulo, B não é nulo, A vem depois
    } else if (regB->dataInicioQueSegue == NULL) {
        return -1; // B é nulo, A não é nulo, B vem depois
    } else {
        // Ambos não nulos, comparar strings
        int comparacao = strcmp(regA->dataInicioQueSegue, regB->dataInicioQueSegue);
        if (comparacao != 0) return comparacao;
    }

    // Critério 4: dataFimQueSegue (crescente) - desempate final
    // Tratar valores nulos: nulos vêm depois dos não-nulos
    if (regA->dataFimQueSegue == NULL && regB->dataFimQueSegue == NULL) {
        return 0; // Ambos nulos, são iguais
    } else if (regA->dataFimQueSegue == NULL) {
        return 1; // A é nulo, B não é nulo, A vem depois
    } else if (regB->dataFimQueSegue == NULL) {
        return -1; // B é nulo, A não é nulo, B vem depois
    } else {
        // Ambos não nulos, comparar strings
        return strcmp(regA->dataFimQueSegue, regB->dataFimQueSegue);
    }
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
    if (fread(&cabecalho.status, sizeof(char), 1, arquivoEntrada) != 1 ||
        fread(&cabecalho.quantidadeSegue, sizeof(int), 1, arquivoEntrada) != 1 ||
        fread(&cabecalho.quantidadeRemovidos, sizeof(int), 1, arquivoEntrada) != 1 ||
        fread(&cabecalho.proxByteOffset, sizeof(long long), 1, arquivoEntrada) != 1) {
        printf("Falha no processamento do arquivo.\n");
        fclose(arquivoEntrada);
        exit(0);
    }

    // Verificar se arquivo está consistente
    if (cabecalho.status != '1') {
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
        /*Liberar memória
        for (i = 0; i < quantidadeRegistros; i++) {
            if (registros[i].dataInicioQueSegue) free(registros[i].dataInicioQueSegue);
            if (registros[i].dataFimQueSegue) free(registros[i].dataFimQueSegue);
        }*/
        free(registros);
        exit(0);
    }

    // Escrever cabeçalho
    cabecalho.status = '0';
    cabecalho.quantidadeSegue = quantidadeRegistros;
    cabecalho.quantidadeRemovidos = 0;
    cabecalho.proxByteOffset = 17; // Tamanho do cabeçalho

    fwrite(&cabecalho.status, sizeof(char), 1, arquivoSaida);
    fwrite(&cabecalho.quantidadeSegue, sizeof(int), 1, arquivoSaida);
    fwrite(&cabecalho.quantidadeRemovidos, sizeof(int), 1, arquivoSaida);
    fwrite(&cabecalho.proxByteOffset, sizeof(long long), 1, arquivoSaida);

    // Escrever todos os registros ordenados
    for (i = 0; i < quantidadeRegistros; i++) {
        escreverRegistroSegue(arquivoSaida, &registros[i]);
    }

    // Atualizar cabeçalho com status consistente
    cabecalho.status = '1';
    cabecalho.proxByteOffset = ftell(arquivoSaida);

    fseek(arquivoSaida, 0, SEEK_SET);
    fwrite(&cabecalho.status, sizeof(char), 1, arquivoSaida);
    fwrite(&cabecalho.quantidadeSegue, sizeof(int), 1, arquivoSaida);
    fwrite(&cabecalho.quantidadeRemovidos, sizeof(int), 1, arquivoSaida);
    fwrite(&cabecalho.proxByteOffset, sizeof(long long), 1, arquivoSaida);

    fclose(arquivoSaida);

    /* Liberar memória
    for (i = 0; i < quantidadeRegistros; i++) {
        if (registros[i].dataInicioQueSegue) free(registros[i].dataInicioQueSegue);
        if (registros[i].dataFimQueSegue) free(registros[i].dataFimQueSegue);
    }*/
    free(registros);

    // Mostrar arquivo ordenado na tela
    binarioNaTela(arquivoOrdenado);
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
        fread(&cabecalho.quantidadeRemovidos, sizeof(int), 1, arquivo) != 1 ||
        fread(&cabecalho.proxByteOffset, sizeof(long long), 1, arquivo) != 1) {
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
            /* Liberar memória já alocada
            for (int i = 0; i < quantidadeRegistros; i++) {
                if (registros[i]->dataInicioQueSegue) free(registros[i]->dataInicioQueSegue);
                if (registros[i]->dataFimQueSegue) free(registros[i]->dataFimQueSegue);
                free(registros[i]);
            }*/
            free(registros);
            fclose(arquivo);
            return NULL;
        }
        
        if (!lerRegistroSegue(arquivo, registros[quantidadeRegistros])) {
            /* Liberar memória já alocada
            for (int i = 0; i <= quantidadeRegistros; i++) {
                if (registros[i]->dataInicioQueSegue) free(registros[i]->dataInicioQueSegue);
                if (registros[i]->dataFimQueSegue) free(registros[i]->dataFimQueSegue);
                free(registros[i]);
            }*/
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
                /* Liberar memória já alocada
                for (int i = 0; i < quantidadeRegistros; i++) {
                    if (registros[i]->dataInicioQueSegue) free(registros[i]->dataInicioQueSegue);
                    if (registros[i]->dataFimQueSegue) free(registros[i]->dataFimQueSegue);
                    free(registros[i]);
                }*/
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
    /*
    for (int i = 0; i < tamanho; i++) {
        if (registros[i] != NULL) {
            if (registros[i]->dataInicioQueSegue) free(registros[i]->dataInicioQueSegue);
            if (registros[i]->dataFimQueSegue) free(registros[i]->dataFimQueSegue);
            free(registros[i]);
        }
    }*/
    free(registros);
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
        char *ptr = entrada;
        while (*ptr && (isdigit(*ptr) || isspace(*ptr))) {
            ptr++;
        }
        
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
        if (valorBusca[0] == '"' && valorBusca[strlen(valorBusca) - 1] == '"') {
            valorBusca[strlen(valorBusca) - 1] = '\0';
            memmove(valorBusca, valorBusca + 1, strlen(valorBusca));
        }
        
        // USAR MÓDULO DE BUSCA MODULARIZADO
        ResultadoBusca *resultado = buscarPessoas(arquivoPessoa, arquivoIndice, nomeCampo, valorBusca);
        
        if (resultado == NULL || resultado->quantidade == 0) {
            printf("Registro inexistente.\n");
            if (resultado != NULL) {
                liberarResultadoBusca(resultado);
            }
            continue;
        }
        
        // Usar o primeiro resultado encontrado
        RegistroPessoa pessoa = resultado->pessoas[0];
        
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
            int i = posicaoAchada;
            while (i >= 0 && registrosSegue[i]->idPessoaQueSegue == idPessoa) {
                // Exibe a pessoa e o segue atual
                printf("Segue a pessoa de codigo: %d\n", registrosSegue[i]->idPessoaQueESeguida);
                
                // Justificativa para seguir
                if (registrosSegue[i]->grauAmizade == 0) {
                    printf("Justificativa para seguir: celebridade\n");
                } else if (registrosSegue[i]->grauAmizade == 1) {
                    printf("Justificativa para seguir: amiga de minha amiga\n");
                } else if (registrosSegue[i]->grauAmizade == 2) {
                    printf("Justificativa para seguir: minha amiga\n");
                } else {
                    printf("Justificativa para seguir: -\n");
                }
                
                printf("Começou a seguir em: %s\n", registrosSegue[i]->dataInicioQueSegue ? registrosSegue[i]->dataInicioQueSegue : "-");
                printf("Parou de seguir em: %s\n", registrosSegue[i]->dataFimQueSegue ? registrosSegue[i]->dataFimQueSegue : "-");
                printf("\n");
                
                i--;
            }
            
            // Achar todos para frente
            i = posicaoAchada + 1;
            while (i < tamanho && registrosSegue[i]->idPessoaQueSegue == idPessoa) {
                // Exibe a pessoa e o segue atual
                printf("Segue a pessoa de codigo: %d\n", registrosSegue[i]->idPessoaQueESeguida);
                
                // Justificativa para seguir
                if (registrosSegue[i]->grauAmizade == 0) {
                    printf("Justificativa para seguir: celebridade\n");
                } else if (registrosSegue[i]->grauAmizade == 1) {
                    printf("Justificativa para seguir: amiga de minha amiga\n");
                } else if (registrosSegue[i]->grauAmizade == 2) {
                    printf("Justificativa para seguir: minha amiga\n");
                } else {
                    printf("Justificativa para seguir: -\n");
                }
                
                printf("Começou a seguir em: %s\n", registrosSegue[i]->dataInicioQueSegue ? registrosSegue[i]->dataInicioQueSegue : "-");
                printf("Parou de seguir em: %s\n", registrosSegue[i]->dataFimQueSegue ? registrosSegue[i]->dataFimQueSegue : "-");
                printf("\n");
                
                i++;
            }
        }
        
        // Liberar memória do resultado de busca
        if (resultado != NULL) {
            liberarResultadoBusca(resultado);
        }
    }
    
    // Liberar memória dos registros segue carregados na RAM
    LiberarRegistrosSegue(registrosSegue, tamanho);
}

