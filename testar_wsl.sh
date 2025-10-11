#!/bin/bash

# Arquivo para salvar resultados
RESULT_FILE="resultados_teste_$(date +%Y%m%d_%H%M%S).txt"

echo "========================================"
echo "    TESTANDO CASOS DE TESTE - ED3 (WSL)"
echo "========================================"
echo "Resultados serão salvos em: $RESULT_FILE"

# Cores para output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Função para salvar no arquivo
save_to_file() {
    echo "$1" | tee -a "$RESULT_FILE"
}

# Inicializa arquivo de resultados
echo "========================================" > "$RESULT_FILE"
echo "RESULTADOS DOS TESTES - $(date)" >> "$RESULT_FILE"
echo "========================================" >> "$RESULT_FILE"
echo "" >> "$RESULT_FILE"

# Compila o programa
echo -e "${BLUE}Compilando o programa...${NC}"
save_to_file "Compilando o programa..."
gcc -o programaTrab *.c
if [ $? -ne 0 ]; then
    echo -e "${RED}ERRO: Falha na compilacao!${NC}"
    save_to_file "ERRO: Falha na compilacao!"
    exit 1
fi

echo -e "${GREEN}Compilacao concluida com sucesso!${NC}"
save_to_file "Compilacao concluida com sucesso!"
echo

# Diretorio dos casos de teste
TEST_DIR="/mnt/c/Users/dansi/Documents/USP/Estrutura de Dados III/Codigo/Casos de Teste - Trabalho Introdutorio"

# Verifica se o diretorio existe
if [ ! -d "$TEST_DIR" ]; then
    echo -e "${RED}ERRO: Diretorio de casos de teste nao encontrado!${NC}"
    echo "Caminho esperado: $TEST_DIR"
    exit 1
fi

# Funcao para comparar saidas
compare_outputs() {
    local test_num=$1
    local program_output=$2
    local expected_output=$3
    
    if [ "$program_output" = "$expected_output" ]; then
        echo -e "${GREEN}✓ TESTE $test_num: PASSOU${NC}"
        return 0
    else
        echo -e "${RED}✗ TESTE $test_num: FALHOU${NC}"
        return 1
    fi
}

# Contadores
passed=0
failed=0

# Testa cada caso de 1 a 8
for i in {1..8}; do
    echo "========================================"
    echo -e "${YELLOW}Testando caso $i...${NC}"
    echo "========================================"
    
    # Salva no arquivo
    save_to_file "========================================"
    save_to_file "Testando caso $i..."
    save_to_file "========================================"
    
    # Verifica se os arquivos existem
    if [ ! -f "$TEST_DIR/$i.in" ] || [ ! -f "$TEST_DIR/$i.out" ]; then
        echo -e "${RED}ERRO: Arquivos do teste $i nao encontrados!${NC}"
        save_to_file "ERRO: Arquivos do teste $i nao encontrados!"
        ((failed++))
        continue
    fi
    
    echo -e "${BLUE}Entrada:${NC}"
    save_to_file "Entrada:"
    cat "$TEST_DIR/$i.in" | tee -a "$RESULT_FILE"
    echo
    save_to_file ""
    
    echo -e "${BLUE}Saida do programa:${NC}"
    save_to_file "Saida do programa:"
    program_output=$(./programaTrab < "$TEST_DIR/$i.in" 2>&1)
    echo "$program_output" | tee -a "$RESULT_FILE"
    echo
    save_to_file ""
    
    echo -e "${BLUE}Saida esperada:${NC}"
    save_to_file "Saida esperada:"
    expected_output=$(cat "$TEST_DIR/$i.out")
    echo "$expected_output" | tee -a "$RESULT_FILE"
    echo
    save_to_file ""
    
    # Compara as saidas
    if compare_outputs $i "$program_output" "$expected_output"; then
        save_to_file "✓ TESTE $i: PASSOU"
        ((passed++))
    else
        save_to_file "✗ TESTE $i: FALHOU"
        ((failed++))
    fi
    
    echo
    save_to_file ""
done

echo "========================================"
echo -e "${BLUE}RESUMO DOS TESTES:${NC}"
echo -e "${GREEN}Passou: $passed${NC}"
echo -e "${RED}Falhou: $failed${NC}"
echo "========================================"

# Salva resumo no arquivo
save_to_file "========================================"
save_to_file "RESUMO DOS TESTES:"
save_to_file "Passou: $passed"
save_to_file "Falhou: $failed"
save_to_file "========================================"

if [ $failed -eq 0 ]; then
    echo -e "${GREEN}Todos os testes passaram! 🎉${NC}"
    save_to_file "Todos os testes passaram! 🎉"
    echo -e "${BLUE}Resultados salvos em: $RESULT_FILE${NC}"
    exit 0
else
    echo -e "${RED}Alguns testes falharam. Verifique as diferencas acima.${NC}"
    save_to_file "Alguns testes falharam. Verifique as diferencas acima."
    echo -e "${BLUE}Resultados salvos em: $RESULT_FILE${NC}"
    exit 1
fi
