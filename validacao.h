#ifndef VALIDACAO_H
#define VALIDACAO_H

#include "estruturas.h"
#include "registro_pessoa.h"

// Funções de validação
int verificarIdExistente(char *binArquivo, int idPessoa);
int validarRegistroPessoa(RegistroPessoa *pessoa);
int validarCampo(char *nomeCampo, char *valor);

#endif

