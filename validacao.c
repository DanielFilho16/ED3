#include "validacao.h"
#include "io_arquivos.h"

#define debug 0

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
    if (!lerCabecalhoPessoa(arquivo, &cabecalho)) {
        fclose(arquivo);
        fprintf(stderr, "Erro ao ler o cabeçalho do arquivo %s\n", binArquivo);
        return 0;
    }
    
    if(debug) fprintf(stderr,"Cabeçalho lido: status=%c, quantidadePessoas=%d, quantidadeRemovidos=%d, proxByteOffset=%lld\n",
        cabecalho.status, cabecalho.quantidadePessoas, cabecalho.quantidadeRemovidos, cabecalho.proxByteOffset);
    
    if (!validarArquivoConsistente(cabecalho.status)) {
        fclose(arquivo);
        fprintf(stderr, "Arquivo %s inconsistente\n", binArquivo);
        return 0;
    }
    
    if(debug) fprintf(stderr,"Verificando existencia do ID %d no arquivo %s\n", idPessoa, binArquivo);
    
    // Percorrer todos os registros
    while (lerRegistroPessoa(arquivo, &pessoa)) {
        if (pessoa.removido == '0' && pessoa.idPessoa == idPessoa) {
            // Liberar memória
            liberarRegistroPessoa(&pessoa);
            fclose(arquivo);
            return 1; // ID já existe
        }

        // Liberar memória
        liberarRegistroPessoa(&pessoa);
    }

    fclose(arquivo);
    return 0; // ID não existe
}

int validarRegistroPessoa(RegistroPessoa *pessoa) {
    if (pessoa == NULL) return 0;
    if (pessoa->idPessoa <= 0) return 0;
    return 1;
}

int validarCampo(char *nomeCampo, char *valor) {
    if (nomeCampo == NULL || valor == NULL) return 0;
    if (strlen(nomeCampo) == 0) return 0;
    return 1;
}

