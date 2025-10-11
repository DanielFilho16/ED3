#include "estruturas.h"

/*
Fun��o para imprimir dados salvos no arquivo em bin�rio
(util para comparar saida no run codes)
*/
void binarioNaTela(char *nomeArquivoBinario) {

	/* Use essa fun��o para compara��o no run.codes. Lembre-se de ter fechado (fclose) o arquivo anteriormente.
	*  Ela vai abrir de novo para leitura e depois fechar (voc� n�o vai perder pontos por isso se usar ela). */

	unsigned long i, cs;
	unsigned char *mb;
	size_t fl;
	FILE *fs;
	if(nomeArquivoBinario == NULL || !(fs = fopen(nomeArquivoBinario, "rb"))) {
		fprintf(stderr, "ERRO AO ESCREVER O BINARIO NA TELA (fun��o binarioNaTela): n�o foi poss�vel abrir o arquivo que me passou para leitura. Ele existe e voc� t� passando o nome certo? Voc� lembrou de fechar ele com fclose depois de usar?\n");
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
	*	Use essa fun��o para ler um campo string delimitado entre aspas (").
	*	Chame ela na hora que for ler tal campo. Por exemplo:
	*
	*	A entrada est� da seguinte forma:
	*		nomeDoCampo "MARIA DA SILVA"
	*
	*	Para ler isso para as strings j� alocadas str1 e str2 do seu programa, voc� faz:
	*		scanf("%s", str1); // Vai salvar nomeDoCampo em str1
	*		scan_quote_string(str2); // Vai salvar MARIA DA SILVA em str2 (sem as aspas)
	*
	*/

	char R;

	while((R = getchar()) != EOF && isspace(R)); // ignorar espa�os, \r, \n...

	if(R == 'N' || R == 'n') { // campo NULO
		getchar(); getchar(); getchar(); // ignorar o "ULO" de NULO.
		strcpy(str, ""); // copia string vazia
	} else if(R == '\"') {
		if(scanf("%[^\"]", str) != 1) { // ler at� o fechamento das aspas
			strcpy(str, "");
		}
		getchar(); // ignorar aspas fechando
	} else if(R != EOF){ // vc t� tentando ler uma string que n�o t� entre aspas! Fazer leitura normal %s ent�o, pois deve ser algum inteiro ou algo assim...
		str[0] = R;
		scanf("%s", &str[1]);
	} else { // EOF
		strcpy(str, "");
	}
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


// Fun��o auxiliar para ler uma linha do CSV usando t�cnicas do utilidades.c
int lerLinhaCSV(FILE *csv, RegistroPessoa *pessoa) {
    char linha[1024];
    char *campos[4];
    int numCampos = 0;

    // Ler uma linha completa
    if (fgets(linha, sizeof(linha), csv) == NULL) {
        return 0; // Fim do arquivo
    }

    // Remover quebra de linha e carriage return
    linha[strcspn(linha, "\r\n")] = '\0';

    // Verificar se a linha est� vazia ap�s remover quebras de linha
    if (strlen(linha) == 0) {
        return 0; // Linha vazia, fim do processamento
    }

    // Dividir a linha pelos campos separados por v�rgula
    // Usando t�cnica de parsing manual para tratar campos vazios
    char *ptr = linha;
    char *inicio = ptr;

    for (int i = 0; i < 4; i++) {
        inicio = ptr;

        // Encontrar a pr�xima v�rgula ou fim da string
        while (*ptr && *ptr != ',') {
            ptr++;
        }

        // Alocar e copiar o campo
        int tamanho = ptr - inicio;
        campos[i] = malloc(tamanho + 1);
        strncpy(campos[i], inicio, tamanho);
        campos[i][tamanho] = '\0';

        numCampos++;

        // Pular a v�rgula se n�o for o fim da string
        if (*ptr == ',') {
            ptr++;
        } else {
            // Se chegou ao fim da string, preencher campos restantes como vazios
            for (int j = i + 1; j < 4; j++) {
                campos[j] = malloc(1);
                campos[j][0] = '\0';
                numCampos++;
            }
            break;
        }
    }

    // Se n�o conseguiu ler pelo menos o primeiro campo, erro
    if (numCampos == 0 || strlen(campos[0]) == 0) {
        // Liberar mem�ria antes de retornar erro
        for (int i = 0; i < numCampos; i++) {
            free(campos[i]);
        }
        return 0;
    }

    // Processar idPessoa (campo obrigat�rio)
    pessoa->idPessoa = atoi(campos[0]);

    // Processar nomePessoa (pode ser vazio)
    if (numCampos > 1 && strlen(campos[1]) > 0) {
        pessoa->tamanhoNomePessoa = strlen(campos[1]);
        pessoa->nomePessoa = malloc(pessoa->tamanhoNomePessoa + 1);
        strcpy(pessoa->nomePessoa, campos[1]);
    } else {
        pessoa->tamanhoNomePessoa = 0;
        pessoa->nomePessoa = NULL;
    }

    // Processar idade (pode ser vazio)
    if (numCampos > 2 && strlen(campos[2]) > 0) {
        pessoa->idadePessoa = atoi(campos[2]);
    } else {
        pessoa->idadePessoa = -1; // Valor nulo
    }

    // Processar nomeUsuario (pode ser vazio)
    if (numCampos > 3 && strlen(campos[3]) > 0) {
        pessoa->tamanhoNomeUsuario = strlen(campos[3]);
        pessoa->nomeUsuario = malloc(pessoa->tamanhoNomeUsuario + 1);
        strcpy(pessoa->nomeUsuario, campos[3]);
    } else {
        pessoa->tamanhoNomeUsuario = 0;
        pessoa->nomeUsuario = NULL;
    }

    // Liberar mem�ria tempor�ria dos campos
    for (int i = 0; i < numCampos; i++) {
        free(campos[i]);
    }

    return 1; // Sucesso
}

// Fun��o auxiliar para calcular o tamanho do registro
int calcularTamanhoRegistro(RegistroPessoa *pessoa) {
    // Tamanho inclui todos os campos ap�s tamanhoRegistro
    //return 4 + 4 + 4 + pessoa->tamanhoNomePessoa + 4 + pessoa->tamanhoNomeUsuario;
    return 1 + 4 + 4 + 4 + 4 + pessoa->tamanhoNomePessoa + 4 + pessoa->tamanhoNomeUsuario;
}

// Fun��o auxiliar para escrever um registro pessoa no arquivo
void escreverRegistroPessoa(FILE *arquivo, RegistroPessoa *pessoa) {
    pessoa->removido = '0'; // N�o removido
    pessoa->tamanhoRegistro = calcularTamanhoRegistro(pessoa);

    // Escrever campos fixos
    fwrite(&pessoa->removido, sizeof(char), 1, arquivo);
    fwrite(&pessoa->tamanhoRegistro, sizeof(int), 1, arquivo);
    fwrite(&pessoa->idPessoa, sizeof(int), 1, arquivo);
    fwrite(&pessoa->idadePessoa, sizeof(int), 1, arquivo);

    // Escrever nomePessoa
    fwrite(&pessoa->tamanhoNomePessoa, sizeof(int), 1, arquivo);
    if (pessoa->tamanhoNomePessoa > 0) {
        fwrite(pessoa->nomePessoa, sizeof(char), pessoa->tamanhoNomePessoa, arquivo);
    }

    // Escrever nomeUsuario
    fwrite(&pessoa->tamanhoNomeUsuario, sizeof(int), 1, arquivo);
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

    // Abrir arquivo bin�rio pessoa
    binFile = fopen(binArquivo, "wb");
    if (binFile == NULL) {
        printf("Falha no processamento do arquivo.\n");
        fclose(csv);
        return;
    }

    // Abrir arquivo de �ndice
    indiceFile = fopen(indiceArquivo, "r+b");
    if (indiceFile == NULL) {
        printf("Falha no processamento do arquivo.\n");
        fclose(csv);
        fclose(binFile);
        return;
    }

    // Inicializar cabe�alho do arquivo pessoa
    cabecalhoPessoa.status = '0';
    cabecalhoPessoa.quantidadePessoas = 0;
    cabecalhoPessoa.quantidadeRemovidos = 0;
    cabecalhoPessoa.proxByteOffset = sizeof(CabecalhoPessoa); // Tamanho do cabe�alho

    // Escrever cabe�alho inicial do arquivo pessoa
    fwrite(&cabecalhoPessoa, sizeof(CabecalhoPessoa), 1, binFile);

    // Ler cabe�alho do arquivo de �ndice e atualizar status para aberto
    fread(&cabecalhoIndice, sizeof(CabecalhoIndice), 1, indiceFile);
    cabecalhoIndice.status = '0';
    fseek(indiceFile, 0, SEEK_SET);
    fwrite(&cabecalhoIndice.status, sizeof(char), 1, indiceFile);

    // Pular a primeira linha (cabe�alho) do CSV
    char linha[1024];
    fgets(linha, sizeof(linha), csv);

    // Ler e processar cada linha do CSV
    while (lerLinhaCSV(csv, &pessoa)) {
        // Salvar offset atual para o �ndice
        offsetAtual = ftell(binFile);

        // Escrever registro no arquivo pessoa
        escreverRegistroPessoa(binFile, &pessoa);

        // Adicionar entrada no �ndice
        registroIndice.idPessoa = pessoa.idPessoa;
        registroIndice.byteOffset = offsetAtual;
        fseek(indiceFile, 0, SEEK_END);

        fwrite(&registroIndice.idPessoa, sizeof(int), 1, indiceFile);
        fwrite(&registroIndice.byteOffset, sizeof(long long),1,indiceFile);

        quantidadePessoas++;

        // Liberar mem�ria alocada
        if (pessoa.nomePessoa) free(pessoa.nomePessoa);
        if (pessoa.nomeUsuario) free(pessoa.nomeUsuario);
    }

    // Atualizar cabe�alho do arquivo pessoa
    cabecalhoPessoa.status = '1';
    cabecalhoPessoa.quantidadePessoas = quantidadePessoas;
    cabecalhoPessoa.proxByteOffset = ftell(binFile);

    fseek(binFile, 0, SEEK_SET);
    fwrite(&cabecalhoPessoa, sizeof(CabecalhoPessoa), 1, binFile);

    // Atualizar status do arquivo de �ndice para fechado
    cabecalhoIndice.status = '1';
    fseek(indiceFile, 0, SEEK_SET);
    fwrite(&cabecalhoIndice.status, sizeof(char), 1, indiceFile);

    // Fechar arquivos
    fclose(csv);
    fclose(binFile);
    fclose(indiceFile);

    // Mostrar conte�do dos arquivos
    binarioNaTela(binArquivo);
    binarioNaTela(indiceArquivo);
}

// Fun��o auxiliar para ler um registro pessoa do arquivo bin�rio
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
            printf("\n");
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

// Fun��o para busca indexada por idPessoa
int buscaIndexada(char *binArquivo, char *indiceArquivo, int idProcurado) {
    FILE *arquivoBin, *arquivoIndice;
    CabecalhoPessoa cabecalhoPessoa;
    CabecalhoIndice cabecalhoIndice;
    RegistroIndice registroIndice;
    RegistroPessoa pessoa;
    int encontrado = 0;

    // Abrir arquivo de �ndice
    arquivoIndice = fopen(indiceArquivo, "rb");
    if (arquivoIndice == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return 0;
    }

    // Ler cabe�alho do �ndice
    if (fread(&cabecalhoIndice, sizeof(CabecalhoIndice), 1, arquivoIndice) != 1) {
        printf("Falha no processamento do arquivo.\n");
        fclose(arquivoIndice);
        return 0;
    }

    // Verificar status do arquivo de �ndice
    if (cabecalhoIndice.status != '1') {
        printf("Falha no processamento do arquivo.\n");
        fclose(arquivoIndice);
        return 0;
    }

    // Abrir arquivo bin�rio
    arquivoBin = fopen(binArquivo, "rb");
    if (arquivoBin == NULL) {
        printf("Falha no processamento do arquivo.\n");
        fclose(arquivoIndice);
        return 0;
    }

    // Ler cabe�alho do arquivo bin�rio
    if (fread(&cabecalhoPessoa, sizeof(CabecalhoPessoa), 1, arquivoBin) != 1) {
        printf("Falha no processamento do arquivo.\n");
        fclose(arquivoBin);
        fclose(arquivoIndice);
        return 0;
    }

    // Verificar status do arquivo bin�rio
    if (cabecalhoPessoa.status != '1') {
        printf("Falha no processamento do arquivo.\n");
        fclose(arquivoBin);
        fclose(arquivoIndice);
        return 0;
    }

    // Percorrer registros do �ndice procurando o ID
    while ((fread(&registroIndice.idPessoa, sizeof(int), 1, arquivoIndice) == 1 )&& (fread(&registroIndice.byteOffset, sizeof(long long), 1, arquivoIndice) == 1  ) ) {
        if (registroIndice.idPessoa == idProcurado) {
            // ID encontrado, ir para o offset no arquivo bin�rio
            fseek(arquivoBin, registroIndice.byteOffset, SEEK_SET);

            if (lerRegistroPessoa(arquivoBin, &pessoa)) {
                // Verificar se n�o foi removido
                if (pessoa.removido == '0') {
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
                    encontrado = 1;
                }

                // Liberar mem�ria
                if (pessoa.nomePessoa) free(pessoa.nomePessoa);
                if (pessoa.nomeUsuario) free(pessoa.nomeUsuario);
            }
            break;
        }
    }

    fclose(arquivoBin);
    fclose(arquivoIndice);
    return encontrado;
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
            if (strcmp(nomeCampo, "nomePessoa") == 0) {
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
            // Busca indexada para idPessoa
            int idProcurado = atoi(valorBusca);
            encontrados = buscaIndexada(binArquivo, indiceArquivo, idProcurado);
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



