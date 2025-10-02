#include "estruturas.h"

/*
Função para imprimir dados salvos no arquivo em binário
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

// Função para criar um arquivo de índice primário vazio
void CREAT_INDEX(char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "wb");
    
    if (arquivo == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }
    
    // Criar e inicializar cabeçalho do índice
    CabecalhoIndice cabecalho;
    cabecalho.status = '0';  // Status aberto
    
    // Preencher lixo com '$'
    for (int i = 0; i < 11; i++) {
        cabecalho.lixo[i] = '$';
    }
    
    // Escrever cabeçalho no arquivo
    fwrite(&cabecalho, sizeof(CabecalhoIndice), 1, arquivo);
    
    // Fechar arquivo e marcar status como fechado
    cabecalho.status = '1';
    fseek(arquivo, 0, SEEK_SET);
    fwrite(&cabecalho.status, sizeof(char), 1, arquivo);
    
    fclose(arquivo);
    
    // Mostrar arquivo na tela usando binarioNaTela
    binarioNaTela(nomeArquivo);
}


// Função auxiliar para ler uma linha do CSV usando técnicas do utilidades.c
int lerLinhaCSV(FILE *csv, RegistroPessoa *pessoa) {
    char linha[1024];
    char *campos[4];
    int numCampos = 0;
    
    // Ler uma linha completa
    if (fgets(linha, sizeof(linha), csv) == NULL) {
        return 0; // Fim do arquivo
    }
    
    // Remover quebra de linha
    linha[strcspn(linha, "\n")] = '\0';
    
    // Dividir a linha pelos campos separados por vírgula
    // Usando técnica de parsing manual para tratar campos vazios
    char *ptr = linha;
    char *inicio = ptr;
    
    for (int i = 0; i < 4; i++) {
        inicio = ptr;
        
        // Encontrar a próxima vírgula ou fim da string
        while (*ptr && *ptr != ',') {
            ptr++;
        }
        
        // Alocar e copiar o campo
        int tamanho = ptr - inicio;
        campos[i] = malloc(tamanho + 1);
        strncpy(campos[i], inicio, tamanho);
        campos[i][tamanho] = '\0';
        
        numCampos++;
        
        // Pular a vírgula se não for o fim da string
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
    
    // Se não conseguiu ler pelo menos o primeiro campo, erro
    if (numCampos == 0) {
        return 0;
    }
    
    // Processar idPessoa (campo obrigatório)
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
    
    // Liberar memória temporária dos campos
    for (int i = 0; i < numCampos; i++) {
        free(campos[i]);
    }
    
    return 1; // Sucesso
}

// Função auxiliar para calcular o tamanho do registro
int calcularTamanhoRegistro(RegistroPessoa *pessoa) {
    return 1 + 4 + 4 + 4 + 4 + pessoa->tamanhoNomePessoa + 4 + pessoa->tamanhoNomeUsuario;
}

// Função auxiliar para escrever um registro pessoa no arquivo
void escreverRegistroPessoa(FILE *arquivo, RegistroPessoa *pessoa) {
    pessoa->removido = '0'; // Não removido
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
    
    // Abrir arquivo binário pessoa
    binFile = fopen(binArquivo, "wb");
    if (binFile == NULL) {
        printf("Falha no processamento do arquivo.\n");
        fclose(csv);
        return;
    }
    
    // Abrir arquivo de índice
    indiceFile = fopen(indiceArquivo, "r+b");
    if (indiceFile == NULL) {
        printf("Falha no processamento do arquivo.\n");
        fclose(csv);
        fclose(binFile);
        return;
    }
    
    // Inicializar cabeçalho do arquivo pessoa
    cabecalhoPessoa.status = '0';
    cabecalhoPessoa.quantidadePessoas = 0;
    cabecalhoPessoa.quantidadeRemovidos = 0;
    cabecalhoPessoa.proxByteOffset = 17; // Tamanho do cabeçalho
    
    // Escrever cabeçalho inicial do arquivo pessoa
    fwrite(&cabecalhoPessoa, sizeof(CabecalhoPessoa), 1, binFile);
    
    // Atualizar status do arquivo de índice para aberto
    cabecalhoIndice.status = '0';
    fwrite(&cabecalhoIndice.status, sizeof(char), 1, indiceFile);
    
    // Pular a primeira linha (cabeçalho) do CSV
    char linha[1024];
    fgets(linha, sizeof(linha), csv);
    
    // Ler e processar cada linha do CSV
    while (lerLinhaCSV(csv, &pessoa)) {
        // Salvar offset atual para o índice
        offsetAtual = ftell(binFile);
        
        // Escrever registro no arquivo pessoa
        escreverRegistroPessoa(binFile, &pessoa);
        
        // Adicionar entrada no índice
        registroIndice.idPessoa = pessoa.idPessoa;
        registroIndice.byteOffset = offsetAtual;
        fseek(indiceFile, 0, SEEK_END);
        fwrite(&registroIndice, sizeof(RegistroIndice), 1, indiceFile);
        
        quantidadePessoas++;
        
        // Liberar memória alocada
        if (pessoa.nomePessoa) free(pessoa.nomePessoa);
        if (pessoa.nomeUsuario) free(pessoa.nomeUsuario);
    }
    
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
    binarioNaTela(indiceArquivo);
}

void SELECT(char *binArquivo) {
    printf("Funcionalidade ainda não implementada.\n");
}

void SELECT_WHERE(char *binArquivo, char *indiceArquivo) {
    char nomeCampo[100];
    char valorBusca[200];
    
    // Usar scan_quote_string para ler entrada interativa
    // Exemplo de entrada: nomePessoa "MARIA DA SILVA"
    scanf("%s", nomeCampo); // lê o nome do campo
    scan_quote_string(valorBusca); // lê o valor entre aspas ou NULO
    
    printf("Busca no arquivo: %s\n", binArquivo);
    printf("Índice usado: %s\n", indiceArquivo);
    printf("Campo: %s\n", nomeCampo);
    printf("Valor: %s\n", strlen(valorBusca) > 0 ? valorBusca : "NULO");
    
    // TODO: Implementar busca real nos registros
    printf("Funcionalidade de busca em desenvolvimento.\n");
}

void INSERT_INTERACTIVE(char *binArquivo, char *indiceArquivo) {
    RegistroPessoa pessoa;
    char buffer[300];
    int id, idade;
    
    printf("=== INSERÇÃO INTERATIVA DE PESSOA ===\n");
    printf("Demonstração do uso de scan_quote_string do utilidades.c\n\n");
    
    // Ler ID da pessoa
    printf("Digite o ID da pessoa: ");
    scanf("%d", &id);
    pessoa.idPessoa = id;
    
    // Usar scan_quote_string para ler nome da pessoa
    printf("Digite o nome da pessoa (entre aspas ou NULO): ");
    scan_quote_string(buffer);
    if (strlen(buffer) > 0) {
        pessoa.tamanhoNomePessoa = strlen(buffer);
        pessoa.nomePessoa = malloc(pessoa.tamanhoNomePessoa + 1);
        strcpy(pessoa.nomePessoa, buffer);
    } else {
        pessoa.tamanhoNomePessoa = 0;
        pessoa.nomePessoa = NULL;
    }
    
    // Ler idade
    printf("Digite a idade (ou -1 para NULO): ");
    scanf("%d", &idade);
    pessoa.idadePessoa = idade;
    
    // Usar scan_quote_string para ler nome de usuário
    printf("Digite o nome de usuário (entre aspas ou NULO): ");
    scan_quote_string(buffer);
    if (strlen(buffer) > 0) {
        pessoa.tamanhoNomeUsuario = strlen(buffer);
        pessoa.nomeUsuario = malloc(pessoa.tamanhoNomeUsuario + 1);
        strcpy(pessoa.nomeUsuario, buffer);
    } else {
        pessoa.tamanhoNomeUsuario = 0;
        pessoa.nomeUsuario = NULL;
    }
    
    // Mostrar dados lidos
    printf("\n=== DADOS LIDOS ===\n");
    printf("ID: %d\n", pessoa.idPessoa);
    printf("Nome: %s\n", pessoa.nomePessoa ? pessoa.nomePessoa : "NULO");
    printf("Idade: %d\n", pessoa.idadePessoa == -1 ? -1 : pessoa.idadePessoa);
    printf("Usuário: %s\n", pessoa.nomeUsuario ? pessoa.nomeUsuario : "NULO");
    
    printf("\nFuncionalidade demonstrativa - dados não foram salvos.\n");
    printf("Para salvar, seria necessário implementar escrita nos arquivos:\n");
    printf("- %s (arquivo binário)\n", binArquivo);
    printf("- %s (arquivo de índice)\n", indiceArquivo);
    
    // Liberar memória
    if (pessoa.nomePessoa) free(pessoa.nomePessoa);
    if (pessoa.nomeUsuario) free(pessoa.nomeUsuario);
}
