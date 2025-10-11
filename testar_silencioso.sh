#!/bin/bash

# Arquivo para salvar resultados
RESULT_FILE="resultados_teste_$(date +%Y%m%d_%H%M%S).txt"

echo "Executando testes... Resultados serão salvos em: $RESULT_FILE"

# Inicializa arquivo de resultados
echo "========================================" > "$RESULT_FILE"
echo "RESULTADOS DOS TESTES - $(date)" >> "$RESULT_FILE"
echo "========================================" >> "$RESULT_FILE"
echo "" >> "$RESULT_FILE"

# Compila o programa
echo "Compilando o programa..." >> "$RESULT_FILE"
gcc -o programaTrab *.c >> "$RESULT_FILE" 2>&1
if [ $? -ne 0 ]; then
    echo "ERRO: Falha na compilacao!" >> "$RESULT_FILE"
    echo "ERRO: Falha na compilacao! Verifique $RESULT_FILE"
    exit 1
fi

echo "Compilacao concluida com sucesso!" >> "$RESULT_FILE"
echo "" >> "$RESULT_FILE"

# Diretorio dos casos de teste
TEST_DIR="/mnt/c/Users/dansi/Documents/USP/Estrutura de Dados III/Codigo/Casos de Teste - Trabalho Introdutorio"

# Verifica se o diretorio existe
if [ ! -d "$TEST_DIR" ]; then
    echo "ERRO: Diretorio de casos de teste nao encontrado!" >> "$RESULT_FILE"
    echo "Caminho esperado: $TEST_DIR" >> "$RESULT_FILE"
    echo "ERRO: Diretorio de casos de teste nao encontrado! Verifique $RESULT_FILE"
    exit 1
fi

# Funcao para comparar saidas
compare_outputs() {
    local test_num=$1
    local program_output=$2
    local expected_output=$3
    
    if [ "$program_output" = "$expected_output" ]; then
        echo "✓ TESTE $test_num: PASSOU" >> "$RESULT_FILE"
        return 0
    else
        echo "✗ TESTE $test_num: FALHOU" >> "$RESULT_FILE"
        return 1
    fi
}

# Contadores
passed=0
failed=0

# Testa cada caso de 1 a 8
for i in {1..8}; do
    echo "========================================" >> "$RESULT_FILE"
    echo "Testando caso $i..." >> "$RESULT_FILE"
    echo "========================================" >> "$RESULT_FILE"
    
    # Verifica se os arquivos existem
    if [ ! -f "$TEST_DIR/$i.in" ] || [ ! -f "$TEST_DIR/$i.out" ]; then
        echo "ERRO: Arquivos do teste $i nao encontrados!" >> "$RESULT_FILE"
        ((failed++))
        continue
    fi
    
    echo "Entrada:" >> "$RESULT_FILE"
    cat "$TEST_DIR/$i.in" >> "$RESULT_FILE"
    echo "" >> "$RESULT_FILE"
    
    echo "Saida do programa:" >> "$RESULT_FILE"
    program_output=$(./programaTrab < "$TEST_DIR/$i.in" 2>&1)
    echo "$program_output" >> "$RESULT_FILE"
    echo "" >> "$RESULT_FILE"
    
    echo "Saida esperada:" >> "$RESULT_FILE"
    expected_output=$(cat "$TEST_DIR/$i.out")
    echo "$expected_output" >> "$RESULT_FILE"
    echo "" >> "$RESULT_FILE"
    
    # Compara as saidas
    if compare_outputs $i "$program_output" "$expected_output"; then
        ((passed++))
    else
        ((failed++))
    fi
    
    echo "" >> "$RESULT_FILE"
done

echo "========================================" >> "$RESULT_FILE"
echo "RESUMO DOS TESTES:" >> "$RESULT_FILE"
echo "Passou: $passed" >> "$RESULT_FILE"
echo "Falhou: $failed" >> "$RESULT_FILE"
echo "========================================" >> "$RESULT_FILE"

if [ $failed -eq 0 ]; then
    echo "Todos os testes passaram! 🎉" >> "$RESULT_FILE"
    echo "SUCESSO: Todos os testes passaram! Verifique $RESULT_FILE"
else
    echo "Alguns testes falharam. Verifique as diferencas acima." >> "$RESULT_FILE"
    echo "ATENCAO: Alguns testes falharam. Verifique $RESULT_FILE"
fi

echo "Resultados salvos em: $RESULT_FILE"
