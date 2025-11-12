#ifndef CSV_PARSER_H
#define CSV_PARSER_H

#include "estruturas.h"

// Funções de leitura de CSV
int ler_csv(FILE *csv, RegistroPessoa *pessoa);
int ler_csv_segue(FILE *csv, RegistroSegue *segue);

// Funções auxiliares de parsing
int parseLinhaCSV(char *linha, char **campos, int maxCampos);
void pularCabecalhoCSV(FILE *csv);

#endif

