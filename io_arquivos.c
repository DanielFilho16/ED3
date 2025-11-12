#include "io_arquivos.h"

// Funções de leitura de cabeçalhos
int lerCabecalhoPessoa(FILE *arquivo, CabecalhoPessoa *cabecalho) {
	if (arquivo == NULL || cabecalho == NULL) return 0;
	
	if (fread(&cabecalho->status, sizeof(char), 1, arquivo) != 1 ||
		fread(&cabecalho->quantidadePessoas, sizeof(int), 1, arquivo) != 1 ||
		fread(&cabecalho->quantidadeRemovidos, sizeof(int), 1, arquivo) != 1 ||
		fread(&cabecalho->proxByteOffset, sizeof(long long), 1, arquivo) != 1) {
		return 0;
	}
	return 1;
}

int lerCabecalhoIndice(FILE *arquivo, CabecalhoIndice *cabecalho) {
	if (arquivo == NULL || cabecalho == NULL) return 0;
	
	if (fread(&cabecalho->status, sizeof(char), 1, arquivo) != 1 ||
		fread(&cabecalho->lixo, 11, 1, arquivo) != 1) {
		return 0;
	}
	return 1;
}

int lerCabecalhoSegue(FILE *arquivo, CabecalhoSegue *cabecalho) {
	if (arquivo == NULL || cabecalho == NULL) return 0;
	
	if (fread(&cabecalho->status, sizeof(char), 1, arquivo) != 1 ||
		fread(&cabecalho->quantidadeSegue, sizeof(int), 1, arquivo) != 1 ||
		fread(&cabecalho->proxRRN, sizeof(int), 1, arquivo) != 1) {
		return 0;
	}
	return 1;
}

// Funções de escrita de cabeçalhos
int escreverCabecalhoPessoa(FILE *arquivo, CabecalhoPessoa *cabecalho) {
	if (arquivo == NULL || cabecalho == NULL) return 0;
	
	fseek(arquivo, 0, SEEK_SET);
	if (fwrite(&cabecalho->status, sizeof(char), 1, arquivo) != 1 ||
		fwrite(&cabecalho->quantidadePessoas, sizeof(int), 1, arquivo) != 1 ||
		fwrite(&cabecalho->quantidadeRemovidos, sizeof(int), 1, arquivo) != 1 ||
		fwrite(&cabecalho->proxByteOffset, sizeof(long long), 1, arquivo) != 1) {
		return 0;
	}
	return 1;
}

int escreverCabecalhoIndice(FILE *arquivo, CabecalhoIndice *cabecalho) {
	if (arquivo == NULL || cabecalho == NULL) return 0;
	
	fseek(arquivo, 0, SEEK_SET);
	if (fwrite(&cabecalho->status, sizeof(char), 1, arquivo) != 1 ||
		fwrite(&cabecalho->lixo, 11, 1, arquivo) != 1) {
		return 0;
	}
	return 1;
}

int escreverCabecalhoSegue(FILE *arquivo, CabecalhoSegue *cabecalho) {
	if (arquivo == NULL || cabecalho == NULL) return 0;
	
	fseek(arquivo, 0, SEEK_SET);
	if (fwrite(&cabecalho->status, sizeof(char), 1, arquivo) != 1 ||
		fwrite(&cabecalho->quantidadeSegue, sizeof(int), 1, arquivo) != 1 ||
		fwrite(&cabecalho->proxRRN, sizeof(int), 1, arquivo) != 1) {
		return 0;
	}
	return 1;
}

// Funções de inicialização de cabeçalhos
void inicializarCabecalhoPessoa(CabecalhoPessoa *cabecalho) {
	if (cabecalho == NULL) return;
	cabecalho->status = '0';
	cabecalho->quantidadePessoas = 0;
	cabecalho->quantidadeRemovidos = 0;
	cabecalho->proxByteOffset = 17; // Tamanho do cabeçalho
}

void inicializarCabecalhoIndice(CabecalhoIndice *cabecalho) {
	if (cabecalho == NULL) return;
	cabecalho->status = '0';
	for (int i = 0; i < 11; i++) {
		cabecalho->lixo[i] = '$';
	}
}

void inicializarCabecalhoSegue(CabecalhoSegue *cabecalho) {
	if (cabecalho == NULL) return;
	cabecalho->status = '0';
	cabecalho->quantidadeSegue = 0;
	cabecalho->proxRRN = 9; // Cabeçalho ocupa 9 bytes
}

// Funções de validação de arquivos
int validarArquivoConsistente(char status) {
	return (status == '1');
}

void marcarArquivoInconsistente(FILE *arquivo, int tipo) {
	if (arquivo == NULL) return;
	
	char status = '0';
	fseek(arquivo, 0, SEEK_SET);
	fwrite(&status, sizeof(char), 1, arquivo);
}

void marcarArquivoConsistente(FILE *arquivo, int tipo) {
	if (arquivo == NULL) return;
	
	char status = '1';
	fseek(arquivo, 0, SEEK_SET);
	fwrite(&status, sizeof(char), 1, arquivo);
}

// Funções auxiliares de arquivo
FILE* abrirArquivoBinario(char *nomeArquivo, char *modo) {
	if (nomeArquivo == NULL || modo == NULL) return NULL;
	return fopen(nomeArquivo, modo);
}

FILE* criarArquivoBinario(char *nomeArquivo) {
	return abrirArquivoBinario(nomeArquivo, "wb");
}

