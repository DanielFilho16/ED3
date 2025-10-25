#ifndef BUSCA_H
#define BUSCA_H

#include "estruturas.h"

// Funções auxiliares de busca
int buscaSequencial(char *binArquivo, char *nomeCampo, char *valorBusca);
int verificarMatch(RegistroPessoa *pessoa, char *nomeCampo, char *valorBusca);

#endif
