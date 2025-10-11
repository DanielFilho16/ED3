#!/bin/bash

echo "========================================"
echo "    TESTE AUTOMATICO - ED3 (WSL)"
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

# Arquivo para salvar resultados
RESULT_FILE="resultados_teste_$(date +%Y%m%d_%H%M%S).txt"

echo "========================================" > "$RESULT_FILE"
echo "RESULTADOS DOS TESTES - $(date)" >> "$RESULT_FILE"
echo "========================================" >> "$RESULT_FILE"
echo "" >> "$RESULT_FILE"

# Contadores
passed=0
failed=0

# Testa cada caso de 1 a 8
for i in {1..8}; do
    echo -e "${YELLOW}Testando caso $i...${NC}"
    
    # Verifica se os arquivos existem
    if [ ! -f "$TEST_DIR/$i.in" ] || [ ! -f "$TEST_DIR/$i.out" ]; then
        echo "ERRO: Arquivos do teste $i nao encontrados!" >> "$RESULT_FILE"
        ((failed++))
        continue
    fi
    
    echo "========================================" >> "$RESULT_FILE"
    echo "TESTE $i" >> "$RESULT_FILE"
    echo "========================================" >> "$RESULT_FILE"
    echo "" >> "$RESULT_FILE"
    
    echo "ENTRADA:" >> "$RESULT_FILE"
    cat "$TEST_DIR/$i.in" >> "$RESULT_FILE"
    echo "" >> "$RESULT_FILE"
    
    echo "SAIDA DO PROGRAMA:" >> "$RESULT_FILE"
    ./programaTrab < "$TEST_DIR/$i.in" >> "$RESULT_FILE" 2>&1
    echo "" >> "$RESULT_FILE"
    
    echo "SAIDA ESPERADA:" >> "$RESULT_FILE"
    cat "$TEST_DIR/$i.out" >> "$RESULT_FILE"
    echo "" >> "$RESULT_FILE"
    echo "========================================" >> "$RESULT_FILE"
    echo "" >> "$RESULT_FILE"
    
    # Compara as saidas
    program_output=$(./programaTrab < "$TEST_DIR/$i.in" 2>&1)
    expected_output=$(cat "$TEST_DIR/$i.out")
    
    if [ "$program_output" = "$expected_output" ]; then
        echo -e "${GREEN}✓ TESTE $i: PASSOU${NC}"
        ((passed++))
    else
        echo -e "${RED}✗ TESTE $i: FALHOU${NC}"
        ((failed++))
    fi
done

echo "========================================" >> "$RESULT_FILE"
echo "RESUMO:" >> "$RESULT_FILE"
echo "Passou: $passed" >> "$RESULT_FILE"
echo "Falhou: $failed" >> "$RESULT_FILE"
echo "========================================" >> "$RESULT_FILE"

echo "========================================"
echo -e "${BLUE}Testes concluidos!${NC}"
echo -e "${BLUE}Resultados salvos em: $RESULT_FILE${NC}"
echo -e "${GREEN}Passou: $passed${NC}"
echo -e "${RED}Falhou: $failed${NC}"
echo "========================================"

# Pergunta se quer abrir o arquivo
echo -e "${YELLOW}Deseja abrir o arquivo de resultados? (s/n)${NC}"
read -r choice
if [[ "$choice" =~ ^[Ss]$ ]]; then
    if command -v nano &> /dev/null; then
        nano "$RESULT_FILE"
    elif command -v vim &> /dev/null; then
        vim "$RESULT_FILE"
    else
        cat "$RESULT_FILE"
    fi
fi
