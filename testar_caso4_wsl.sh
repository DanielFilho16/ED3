#!/bin/bash

echo "========================================"
echo "    TESTANDO APENAS CASO 4 - ED3 (WSL)"
echo "========================================"

# Cores para output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Compila o programa
echo -e "${BLUE}Compilando o programa...${NC}"
gcc -o programaTrab *.c
if [ $? -ne 0 ]; then
    echo -e "${RED}ERRO: Falha na compilacao!${NC}"
    exit 1
fi

echo -e "${GREEN}Compilacao concluida com sucesso!${NC}"
echo

# Diretorio dos casos de teste
TEST_DIR="/mnt/c/Users/dansi/Documents/USP/Estrutura de Dados III/Codigo/Casos de Teste - Trabalho Introdutorio"

# Verifica se o diretorio existe
if [ ! -d "$TEST_DIR" ]; then
    echo -e "${RED}ERRO: Diretorio de casos de teste nao encontrado!${NC}"
    echo "Caminho esperado: $TEST_DIR"
    exit 1
fi

# Verifica se os arquivos do caso 4 existem
if [ ! -f "$TEST_DIR/4.in" ] || [ ! -f "$TEST_DIR/4.out" ]; then
    echo -e "${RED}ERRO: Arquivos do teste 4 nao encontrados!${NC}"
    exit 1
fi

echo "========================================"
echo -e "${YELLOW}Testando caso 4...${NC}"
echo "========================================"

echo -e "${BLUE}Entrada (4.in):${NC}"
cat "$TEST_DIR/4.in"
echo

echo -e "${BLUE}Executando programa...${NC}"
program_output=$(./programaTrab < "$TEST_DIR/4.in" 2>&1)
echo -e "${BLUE}Saida do programa:${NC}"
echo "$program_output"
echo

echo -e "${BLUE}Saida esperada (4.out):${NC}"
expected_output=$(cat "$TEST_DIR/4.out")
echo "$expected_output"
echo

# Compara as saidas
if [ "$program_output" = "$expected_output" ]; then
    echo -e "${GREEN}✓ TESTE 4: PASSOU${NC}"
    echo "========================================"
    echo -e "${GREEN}O caso 4 passou com sucesso! 🎉${NC}"
    echo "========================================"
    exit 0
else
    echo -e "${RED}✗ TESTE 4: FALHOU${NC}"
    echo
    echo -e "${YELLOW}Diferenças encontradas:${NC}"
    echo "Programa: '$program_output'"
    echo "Esperado: '$expected_output'"
    echo "========================================"
    echo -e "${RED}O caso 4 falhou. Verifique as diferenças acima.${NC}"
    echo "========================================"
    exit 1
fi
