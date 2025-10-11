# Guia de Testes para WSL - Estrutura de Dados III

## 🐧 Configuração Inicial

### 1. Acesse o WSL
```bash
# No terminal do Windows, execute:
wsl
```

### 2. Navegue até o diretório do projeto
```bash
cd /mnt/c/Users/dansi/Documents/USP/Estrutura\ de\ Dados\ III/Codigo/ED3
```

### 3. Instale dependências (se necessário)
```bash
make install-deps
```

## 🚀 Como Executar os Testes

### Opção 1: Teste Interativo (Recomendado)
```bash
# Torna o script executável e roda
chmod +x testar_wsl.sh
./testar_wsl.sh
```

**Características:**
- ✅ Mostra entrada, saída do programa e saída esperada
- ✅ Compara automaticamente os resultados
- ✅ Cores para facilitar a leitura
- ✅ Contador de testes passou/falhou
- ✅ Para entre cada teste para análise

### Opção 2: Teste Automático
```bash
# Torna o script executável e roda
chmod +x testar_automatico_wsl.sh
./testar_automatico_wsl.sh
```

**Características:**
- ✅ Executa todos os testes automaticamente
- ✅ Salva resultados em arquivo com timestamp
- ✅ Compara automaticamente os resultados
- ✅ Resumo final com estatísticas

### Opção 3: Usando Makefile
```bash
# Compilar e testar
make test

# Ou teste automático
make test-auto

# Compilar com debug
make debug

# Compilar com sanitizers (detecção de erros)
make sanitize
```

## 📁 Estrutura dos Casos de Teste

Os casos de teste estão localizados em:
```
/mnt/c/Users/dansi/Documents/USP/Estrutura de Dados III/Codigo/Casos de Teste - Trabalho Introdutorio/
```

**Arquivos disponíveis:**
- `1.in` / `1.out` - Teste da funcionalidade 1
- `2.in` / `2.out` - Teste da funcionalidade 2
- `3.in` / `3.out` - Teste da funcionalidade 2 (caso de erro)
- `4.in` / `4.out` - Teste da funcionalidade 3
- `5.in` / `5.out` - Teste da funcionalidade 4
- `6.in` / `6.out` - Teste da funcionalidade 5
- `7.in` / `7.out` - Teste adicional
- `8.in` / `8.out` - Teste adicional

## 🔧 Comandos Úteis

### Compilação
```bash
# Compilação básica
make

# Compilação com debug
make debug

# Compilação com sanitizers
make sanitize
```

### Execução Manual
```bash
# Executar um teste específico
./programaTrab < "/mnt/c/Users/dansi/Documents/USP/Estrutura de Dados III/Codigo/Casos de Teste - Trabalho Introdutorio/1.in"

# Executar o programa interativamente
./programaTrab
```

### Limpeza
```bash
# Remove arquivos compilados e resultados
make clean
```

## 🐛 Debugging

### Com Sanitizers
```bash
make sanitize
./programaTrab < entrada.in
```

### Com GDB
```bash
make debug
gdb ./programaTrab
```

### Valgrind (se disponível)
```bash
make debug
valgrind --leak-check=full ./programaTrab < entrada.in
```

## 📊 Interpretando os Resultados

### ✅ Teste Passou
```
✓ TESTE 1: PASSOU
```

### ❌ Teste Falhou
```
✗ TESTE 2: FALHOU
```

### 📈 Resumo Final
```
========================================
RESUMO DOS TESTES:
Passou: 6
Falhou: 2
========================================
```

## 🚨 Solução de Problemas

### Erro de Compilação
```bash
# Verifique se tem as dependências
make install-deps

# Compile com mais warnings
gcc -Wall -Wextra -std=c99 -o programaTrab *.c
```

### Arquivos de Teste Não Encontrados
```bash
# Verifique o caminho
ls -la "/mnt/c/Users/dansi/Documents/USP/Estrutura de Dados III/Codigo/Casos de Teste - Trabalho Introdutorio/"

# Ajuste o caminho nos scripts se necessário
```

### Permissões
```bash
# Dê permissão de execução
chmod +x *.sh
```

## 💡 Dicas

1. **Use o teste interativo** para debug detalhado
2. **Use o teste automático** para verificação rápida
3. **Compile com sanitizers** para detectar erros de memória
4. **Verifique os arquivos de entrada** se algum teste falhar
5. **Use `make clean`** antes de recompilar se houver problemas
