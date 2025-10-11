#include "estruturas.h"

// Função de comparação para ordenação por idPessoa
int compararIdPessoa(const void *a, const void *b) {
    const RegistroIndice *regA = (const RegistroIndice *)a;
    const RegistroIndice *regB = (const RegistroIndice *)b;
    
    if (regA->idPessoa < regB->idPessoa) return -1;
    if (regA->idPessoa > regB->idPessoa) return 1;
    return 0;
}

/*
Fun��o para imprimir dados salvos no arquivo em bin�rio
(util para comparar saida no run codes)
*/
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

// Função para ler uma linha CSV usando scan_quote_string
int ler_csv(FILE *csv, RegistroPessoa *pessoa) {
    char linha[1024];
    char temp[1024];
    int pos = 0;
    
    // Ler uma linha completa do arquivo CSV
    if (fgets(linha, sizeof(linha), csv) == NULL) {
        return 0; // Fim do arquivo
    }
    
    // Remover quebra de linha
    linha[strcspn(linha, "\r\n")] = '\0';
    
    // Verificar se a linha está vazia
    if (strlen(linha) == 0) {
        return 0;
    }
    
    // Simular entrada do stdin para scan_quote_string
    // Criar um buffer temporário com a linha
    strcpy(temp, linha);
    
    // Usar sscanf para extrair os campos separados por vírgula
    // Formato real: idPessoa,nomePessoa,idade,nomeUsuario
    
    // Ler idPessoa (primeiro campo)
    if (sscanf(temp, "%d", &pessoa->idPessoa) != 1) {
        return 0;
    }
    
    // Avançar para o próximo campo (pular vírgula)
    char *ptr = strchr(temp, ',');
    if (ptr == NULL) return 0;
    ptr++;
    
    // Ler nomePessoa (segundo campo) - pode estar entre aspas
    char nomePessoaTemp[256];
    if (ptr[0] == '"') {
        // Campo entre aspas
        if (sscanf(ptr, "\"%[^\"]\"", nomePessoaTemp) != 1) {
            return 0;
        }
        // Avançar para o próximo campo
        ptr = strchr(ptr + 1, '"');
        if (ptr == NULL) return 0;
        ptr = strchr(ptr, ',');
        if (ptr == NULL) return 0;
        ptr++;
    } else {
        // Campo sem aspas - pode estar vazio
        if (sscanf(ptr, "%[^,]", nomePessoaTemp) != 1) {
            // Se não conseguiu ler, verificar se é campo vazio
            if (ptr[0] == ',') {
                strcpy(nomePessoaTemp, ""); // Campo vazio
            } else {
                return 0; // Erro real na leitura
            }
        }
        // Avançar para o próximo campo
        ptr = strchr(ptr, ',');
        if (ptr == NULL) return 0;
        ptr++;
    }
    
    // Ler idadePessoa (terceiro campo) - pode estar vazio
    if (sscanf(ptr, "%d", &pessoa->idadePessoa) != 1) {
        // Se não conseguiu ler um inteiro, verificar se é campo vazio
        if (ptr[0] == ',') {
            pessoa->idadePessoa = -1; // Marcar como inválido/vazio
        } else {
            return 0; // Erro real na leitura
        }
    }
    
    // Avançar para o próximo campo (pular vírgula)
    ptr = strchr(ptr, ',');
    if (ptr == NULL) return 0;
    ptr++;
    
    // Ler nomeUsuario (quarto campo) - pode estar entre aspas
    char nomeUsuarioTemp[256];
    if (ptr[0] == '"') {
        // Campo entre aspas
        if (sscanf(ptr, "\"%[^\"]\"", nomeUsuarioTemp) != 1) {
            return 0;
        }
    } else {
        // Campo sem aspas - pode estar vazio
        if (sscanf(ptr, "%s", nomeUsuarioTemp) != 1) {
            // Se não conseguiu ler, verificar se é campo vazio
            if (ptr[0] == '\0' || ptr[0] == '\n' || ptr[0] == '\r') {
                strcpy(nomeUsuarioTemp, ""); // Campo vazio
            } else {
                return 0; // Erro real na leitura
            }
        }
    }
    
    // Alocar memória e copiar strings
    pessoa->tamanhoNomePessoa = strlen(nomePessoaTemp);
    if (pessoa->tamanhoNomePessoa > 0) {
        pessoa->nomePessoa = malloc(pessoa->tamanhoNomePessoa + 1);
        if (pessoa->nomePessoa == NULL) {
            return 0;
        }
        strcpy(pessoa->nomePessoa, nomePessoaTemp);
    } else {
        pessoa->nomePessoa = NULL;
    }
    
    pessoa->tamanhoNomeUsuario = strlen(nomeUsuarioTemp);
    if (pessoa->tamanhoNomeUsuario > 0) {
        pessoa->nomeUsuario = malloc(pessoa->tamanhoNomeUsuario + 1);
        if (pessoa->nomeUsuario == NULL) {
            if (pessoa->nomePessoa) free(pessoa->nomePessoa);
            return 0;
        }
        strcpy(pessoa->nomeUsuario, nomeUsuarioTemp);
    } else {
        pessoa->nomeUsuario = NULL;
    }
    
    return 1; // Sucesso
}

// Fun��o para criar um arquivo de �ndice prim�rio vazio
void CREAT_INDEX(char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "wb");

    if (arquivo == NULL) { //conferir cria��o do arquivo
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // Criar e inicializar cabe�alho do �ndice
    CabecalhoIndice cabecalho;
    cabecalho.status = '0';  // Status aberto

    // Preencher lixo com '$'
    for (int i = 0; i < 11; i++) {
        cabecalho.lixo[i] = '$';
    }

    // Escrever cabe�alho no arquivo
    fwrite(&cabecalho.status, 1 , 1, arquivo);
    fwrite(&cabecalho.lixo,11,1,arquivo);


    // Fechar arquivo e marcar status como fechado
    cabecalho.status = '1';
    fseek(arquivo, 0, SEEK_SET);
    fwrite(&cabecalho.status, sizeof(char), 1, arquivo);

    fclose(arquivo);

    // Mostrar arquivo na tela usando binarioNaTela
    binarioNaTela(nomeArquivo);
}




// Função auxiliar para calcular o tamanho do registro
int calcularTamanhoRegistro(RegistroPessoa *pessoa) {
    // Tamanho inclui todos os campos ap�s tamanhoRegistro
    return 1 + 4 + 4 + 4 + 4 + pessoa->tamanhoNomePessoa + 4 + pessoa->tamanhoNomeUsuario;
}

// Função auxiliar para escrever um registro pessoa no arquivo
void escreverRegistroPessoa(FILE *arquivo, RegistroPessoa *pessoa) {
    pessoa->removido = '0'; // 
    pessoa->tamanhoRegistro = calcularTamanhoRegistro(pessoa);

    // Escrever campos fixos
    fwrite(&pessoa->removido,   sizeof(char), 1, arquivo);
    fwrite(&pessoa->tamanhoRegistro, sizeof(int), 1, arquivo);
    fwrite(&pessoa->idPessoa, sizeof(int), 1, arquivo);
    fwrite(&pessoa->idadePessoa, sizeof(int), 1, arquivo);
    fwrite(&pessoa->tamanhoNomePessoa, sizeof(int), 1, arquivo);
    fwrite(&pessoa->tamanhoNomeUsuario, sizeof(int), 1, arquivo);



    // Escrever nomePessoa
    if (pessoa->tamanhoNomePessoa > 0) {
        fwrite(pessoa->nomePessoa, sizeof(char), pessoa->tamanhoNomePessoa, arquivo);
    }

    // Escrever nomeUsuario
    if (pessoa->tamanhoNomeUsuario > 0) {
        fwrite(pessoa->nomeUsuario, sizeof(char), pessoa->tamanhoNomeUsuario, arquivo);
    }
}

// Funcionalidade 2: Criar tabela a partir de CSV
void CREAT_TABLE(char *csvArquivo, char *binArquivo, char *indiceArquivo) {
    FILE *csv, *binFile, *indiceFile;
    CabecalhoPessoa cabecalhoPessoa;
    CabecalhoIndice cabecalhoIndice;
    RegistroPessoa pessoa;
    RegistroIndice registroIndice;
    int quantidadePessoas = 0;
    long long offsetAtual;

    // Abrir arquivo CSV
    csv = fopen(csvArquivo, "r");
    if (csv == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // Abrir arquivo binário pessoa
    binFile = fopen(binArquivo, "wb");
    if (binFile == NULL) {
        printf("Falha no processamento do arquivo.\n");
        fclose(csv);
        return;
    }

    // Verificar se o arquivo de índice existe, se não existir, criar
    indiceFile = fopen(indiceArquivo, "r+b");
    if (indiceFile == NULL) {
        // Arquivo de índice não existe, criar um novo
        indiceFile = fopen(indiceArquivo, "w+b");
        if (indiceFile == NULL) {
            printf("Falha no processamento do arquivo.\n");
            fclose(csv);
            fclose(binFile);
            return;
        }
        
        // Inicializar cabeçalho do índice para arquivo novo
        cabecalhoIndice.status = '0';
        for (int i = 0; i < 11; i++) {
            cabecalhoIndice.lixo[i] = '$';
        }
        fwrite(&cabecalhoIndice, sizeof(CabecalhoIndice), 1, indiceFile);
		
    } else {
        // Arquivo de índice existe, ler cabeçalho
        fread(&cabecalhoIndice, sizeof(CabecalhoIndice), 1, indiceFile);
        cabecalhoIndice.status = '0';
        fseek(indiceFile, 0, SEEK_SET);
        fwrite(&cabecalhoIndice.status, sizeof(char), 1, indiceFile);
        // Posicionar cursor após o cabeçalho para escrever os registros
        fseek(indiceFile, sizeof(CabecalhoIndice), SEEK_SET);
    }

    // Inicializar cabeçalho do arquivo pessoa
    cabecalhoPessoa.status = '0';
    cabecalhoPessoa.quantidadePessoas = 0;
    cabecalhoPessoa.quantidadeRemovidos = 0;
    cabecalhoPessoa.proxByteOffset = sizeof(CabecalhoPessoa); // Tamanho do cabeçalho

    // Escrever cabeçalho inicial do arquivo pessoa
    fwrite(&cabecalhoPessoa, sizeof(CabecalhoPessoa), 1, binFile);

    // Pular a primeira linha (cabeçalho) do CSV
    char linha[1024];
    fgets(linha, sizeof(linha), csv);

    // Primeira passada: coletar todos os registros e escrever no arquivo pessoa
    RegistroIndice *registrosIndice = NULL;
    int capacidade = 100;
    int tamanhoAtual = 0;
    
    // Alocar array inicial
    registrosIndice = malloc(capacidade * sizeof(RegistroIndice));
    if (registrosIndice == NULL) {
        printf("Falha no processamento do arquivo.\n");
        fclose(csv);
        fclose(binFile);
        fclose(indiceFile);
        return;
    }

    // Ler e processar cada linha do CSV usando scan_quote_string
    while (ler_csv(csv, &pessoa)) {
        // Salvar offset atual para o índice
        offsetAtual = ftell(binFile);

        // Escrever registro no arquivo pessoa
        escreverRegistroPessoa(binFile, &pessoa);

        // Verificar se precisa expandir o array
        if (tamanhoAtual >= capacidade) {
            capacidade *= 2;
            registrosIndice = realloc(registrosIndice, capacidade * sizeof(RegistroIndice));
            if (registrosIndice == NULL) {
                printf("Falha no processamento do arquivo.\n");
                fclose(csv);
                fclose(binFile);
                fclose(indiceFile);
                return;
            }
        }

        // Adicionar entrada no array de índices
        registrosIndice[tamanhoAtual].idPessoa = pessoa.idPessoa;
        registrosIndice[tamanhoAtual].byteOffset = offsetAtual;
        tamanhoAtual++;

        quantidadePessoas++;

        // Liberar memória alocada
        if (pessoa.nomePessoa) free(pessoa.nomePessoa);
        if (pessoa.nomeUsuario) free(pessoa.nomeUsuario);
    }

    // ORDENAR os registros de índice por idPessoa em ordem crescente
    qsort(registrosIndice, tamanhoAtual, sizeof(RegistroIndice), compararIdPessoa);

    // Escrever registros ordenados no arquivo de índice
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
    fwrite(&cabecalhoPessoa, sizeof(CabecalhoPessoa), 1, binFile);

    // Atualizar status do arquivo de índice para fechado
    cabecalhoIndice.status = '1';
    fseek(indiceFile, 0, SEEK_SET);
    fwrite(&cabecalhoIndice.status, sizeof(char), 1, indiceFile);

    // Fechar arquivos
    fclose(csv);
    fclose(binFile);
    fclose(indiceFile);

    // Mostrar conteúdo dos arquivos
    binarioNaTela(binArquivo);
    //esse ta com problema
    binarioNaTela(indiceArquivo);
}

int lerRegistroPessoa(FILE *arquivo, RegistroPessoa *pessoa) {
    // Ler campo removido
    if (fread(&pessoa->removido, sizeof(char), 1, arquivo) != 1) {
        return 0; // Fim do arquivo ou erro
    }

    // Ler tamanho do registro
    if (fread(&pessoa->tamanhoRegistro, sizeof(int), 1, arquivo) != 1) {
        return 0;
    }

    // Ler campos fixos
    if (fread(&pessoa->idPessoa, sizeof(int), 1, arquivo) != 1) return 0;
    if (fread(&pessoa->idadePessoa, sizeof(int), 1, arquivo) != 1) return 0;

    // Ler nomePessoa
    if (fread(&pessoa->tamanhoNomePessoa, sizeof(int), 1, arquivo) != 1) return 0;

    if (pessoa->tamanhoNomePessoa > 0) {
        pessoa->nomePessoa = malloc(pessoa->tamanhoNomePessoa + 1);
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
        if (fread(pessoa->nomeUsuario, sizeof(char), pessoa->tamanhoNomeUsuario, arquivo) != pessoa->tamanhoNomeUsuario) {
            if (pessoa->nomePessoa) free(pessoa->nomePessoa);
            free(pessoa->nomeUsuario);
            return 0;
        }
        pessoa->nomeUsuario[pessoa->tamanhoNomeUsuario] = '\0';
    } else {
        pessoa->nomeUsuario = NULL;
    }

    return 1; // Sucesso
}

// Funcionalidade 3: Listar todos os registros do arquivo bin�rio
void SELECT(char *binArquivo) {
    FILE *arquivo;
    CabecalhoPessoa cabecalho;
    RegistroPessoa pessoa;
    int registrosEncontrados = 0;

    // Abrir arquivo bin�rio para leitura
    arquivo = fopen(binArquivo, "rb");
    if (arquivo == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // Ler cabe�alho
    /*if (fread(&cabecalho, sizeof(CabecalhoPessoa), 1, arquivo) != 1) {
        printf("Falha no processamento do arquivo.\n");
        fclose(arquivo);
        return;
    }*/

    if (fread(&cabecalho.status, sizeof(char), 1, arquivo) != 1) {
        printf("Falha no processamento do arquivo.\n");
        fclose(arquivo);
        return;
    }
    if (fread(&cabecalho.quantidadePessoas, sizeof(int), 1, arquivo) != 1) {
        printf("Falha no processamento do arquivo.\n");
        fclose(arquivo);
        return;
    }
    if (fread(&cabecalho.quantidadeRemovidos, sizeof(int), 1, arquivo) != 1) {
        printf("Falha no processamento do arquivo.\n");
        fclose(arquivo);
        return;
    }
    if (fread(&cabecalho.proxByteOffset, sizeof(long long), 1, arquivo) != 1) {
        printf("Falha no processamento do arquivo.\n");
        fclose(arquivo);
        return;
    }





    // Verificar se o arquivo est� consistente (status = '1')
    if (cabecalho.status != '1') {
        printf("Falha no processamento do arquivo.\n");
        fclose(arquivo);
        return;
    }

    // Percorrer todos os registros sequencialmente
    while (lerRegistroPessoa(arquivo, &pessoa)) {
        // Verificar se o registro n�o foi logicamente removido
        if (pessoa.removido == '0') {
            registrosEncontrados++;

            // Exibir dados do registro conforme especifica��o
            printf("Dados da pessoa de codigo %d\n", pessoa.idPessoa);

            // Nome (ou "-" se nulo)
            printf("Nome: %s\n", pessoa.nomePessoa ? pessoa.nomePessoa : "-");

            // Idade (ou "-" se nulo/inv�lido)
            if (pessoa.idadePessoa == -1) {
                printf("Idade: -\n");
            } else {
                printf("Idade: %d\n", pessoa.idadePessoa);
            }

            // Usuario (ou "-" se nulo)
            printf("Usuario: %s\n", pessoa.nomeUsuario ? pessoa.nomeUsuario : "-");

            // Linha em branco entre registros
            printf("\n\n");
        }

        // Liberar mem�ria alocada
        if (pessoa.nomePessoa) {
            free(pessoa.nomePessoa);
            pessoa.nomePessoa = NULL;
        }
        if (pessoa.nomeUsuario) {
            free(pessoa.nomeUsuario);
            pessoa.nomeUsuario = NULL;
        }
    }

    // Se n�o encontrou nenhum registro v�lido
    if (registrosEncontrados == 0) {
        printf("Registro inexistente.\n");
    }

    fclose(arquivo);
}


// Fun��o para busca sequencial por outros campos
int buscaSequencial(char *binArquivo, char *nomeCampo, char *valorBusca) {
    FILE *arquivo;
    CabecalhoPessoa cabecalho;
    RegistroPessoa pessoa;
    int encontrados = 0;

    // Abrir arquivo bin�rio
    arquivo = fopen(binArquivo, "rb");
    if (arquivo == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return 0;
    }

    // Ler cabe�alho
    if (fread(&cabecalho, sizeof(CabecalhoPessoa), 1, arquivo) != 1) {
        printf("Falha no processamento do arquivo.\n");
        fclose(arquivo);
        return 0;
    }

    // Verificar status do arquivo
    if (cabecalho.status != '1') {
        printf("Falha no processamento do arquivo.\n");
        fclose(arquivo);
        return 0;
    }

    // Percorrer todos os registros sequencialmente
    while (lerRegistroPessoa(arquivo, &pessoa)) {
        // Verificar se o registro n�o foi removido
        if (pessoa.removido == '0') {
            int match = 0;

            // Verificar qual campo est� sendo procurado
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
                // Exibir dados do registro
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

        // Liberar mem�ria
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

void SELECT_WHERE(char *binArquivo, char *indiceArquivo, int n) {
    int totalEncontrados = 0;

    for (int i = 0; i < n; i++) {
        char entrada[300];
        char nomeCampo[100];
        char valorBusca[200];

        // Ler entrada no formato: nomeCampo=valorCampo (pode conter espa�os)
        if (fgets(entrada, sizeof(entrada), stdin) == NULL) {
            printf("Falha no processamento do arquivo.\n");
            return;
        }

        // Remover quebra de linha
        entrada[strcspn(entrada, "\r\n")] = '\0';

        // Pular linhas vazias
        if (strlen(entrada) == 0) {
            i--; // Repetir esta itera��o
            continue;
        }

        // Procurar o s�mbolo '=' para separar campo e valor
        char *separador = strchr(entrada, '=');
        if (separador == NULL) {
            printf("Falha no processamento do arquivo.\n");
            return;
        }

        // Separar nomeCampo e valorCampo
        *separador = '\0';
        char *nomeCampoPtr = entrada;

        // Verificar se h� numera��o no in�cio (ex: "1 idPessoa=96")
        // Pular n�meros e espa�os no in�cio
        while (*nomeCampoPtr && (isdigit(*nomeCampoPtr) || isspace(*nomeCampoPtr))) {
            nomeCampoPtr++;
        }

        strcpy(nomeCampo, nomeCampoPtr);
        strcpy(valorBusca, separador + 1);

        // Se o valor estiver entre aspas, remover as aspas
        if (valorBusca[0] == '"' && valorBusca[strlen(valorBusca) - 1] == '"') {
            valorBusca[strlen(valorBusca) - 1] = '\0';
            memmove(valorBusca, valorBusca + 1, strlen(valorBusca));
        }

        int encontrados = 0;

        // Decidir se usar busca indexada ou sequencial
        if (strcmp(nomeCampo, "idPessoa") == 0) {
            // Busca sequencial para idPessoa (temporário)
            int idProcurado = atoi(valorBusca);
            encontrados = buscaSequencial(binArquivo, "idPessoa", valorBusca);
        } else {
            // Busca sequencial para outros campos
            encontrados = buscaSequencial(binArquivo, nomeCampo, valorBusca);
        }

        totalEncontrados += encontrados;
    }

    // Se nenhum registro foi encontrado em todas as buscas
    if (totalEncontrados == 0) {
        printf("Registro inexistente.\n");
    }
}





