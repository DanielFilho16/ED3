// funcionalidades.c - Refatorado para usar módulos especializados
// Este arquivo agora apenas inclui os headers dos módulos CRUD
// Todas as implementações foram movidas para os módulos especializados

#include "estruturas.h"
#include "busca.h"
#include "crud_pessoa.h"
#include "crud_segue.h"

// Este arquivo mantém apenas as declarações necessárias para compatibilidade
// Todas as funções CRUD foram movidas para:
// - crud_pessoa.c/h: CREAT_INDEX, CREAT_TABLE, SELECT, SELECT_WHERE, INSERT_INTO, DELETE_FROM, UPDATE
// - crud_segue.c/h: CREAT_TABLE_SEGUE, ORDER_BY, JOIN
// 
// Funções auxiliares foram movidas para:
// - utils.c/h: binarioNaTela, scan_quote_string, removerAspas, pularNumeroLinha
// - io_arquivos.c/h: leitura/escrita de cabeçalhos, validação de arquivos
// - csv_parser.c/h: ler_csv, ler_csv_segue, pularCabecalhoCSV
// - registro_pessoa.c/h: manipulação de registros Pessoa
// - registro_segue.c/h: manipulação de registros Segue
// - indice.c/h: gerenciamento de índice
// - validacao.c/h: validações

// O código agora está completamente modularizado e organizado em módulos especializados
