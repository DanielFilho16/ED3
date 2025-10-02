// Dupla 31
// Aluno	: Daniel Dias Silva Filho ------------ N° USP: 13677114
// Aluno : Kaue Antônio Maranzano de Araújo --- N° USP: 14669684

#include "estruturas.h"

int main() {
	
	int funcionalidade; // declara uma variavel para salvar a escolha do usuário
	char arg1[100], arg2[100], arg3[100]; // declara 3 possíveis argumentos para o usuário passar após a escolha 
	
	scanf("%d", &funcionalidade); // recebe a escolha do usuário pelo STDin

	switch (funcionalidade) {

		case 1: // se a funcionalidade escolhida for a numero 1:
			scanf("%s", arg1);	// recebe o nome do arquivo de índice primario a ser criado 
			CREAT_INDEX(arg1);	// chama a função de criar arquivo de indice primario passando o unico argumento necessario 
			break;

		case 2: // se a funcionalidade escolhida for a numero 2:
			scanf("%s", arg1); // recebe o nome do arquivo csv de entrada
			scanf("%s", arg2); // recebe o nome do arquivo binário de saida
			scanf("%s", arg3); // recebe o nome do arquivo de índice primário
			CREAT_TABLE(arg1, arg2, arg3); // chama a função de criar uma tabela passando os 3 argumentos necessarios
			break;

		case 3: // se a funcionalidade escolhida for a numero 3 :
			scanf("%s", arg1); // recebe o nome do arquivo binário a ser listado            
			SELECT(arg1); // chama a função de listar tabela passando o único argumento necessário
			break;
		
		case 4: // se a funcionalidade escolhida for a numero 4:
			scanf("%s", arg1); // recebe o nome do arquivo binário de entrada
			scanf("%s", arg2); // recebe o nome do arquivo binário de índice primario 
			scanf("%s", arg3); // recebe a quantidade de vezes que a busca deve ser feita
			SELECT_WHERE(arg1, arg2); // chama a função de buscar e listar pessoas se baseando em critérios específicos
			break;

		case 5: // se a funcionalidade escolhida for a numero 5:
			scanf("%s", arg1); // recebe o nome do arquivo binário
			scanf("%s", arg2); // recebe o nome do arquivo de índice
			INSERT_INTERACTIVE(arg1, arg2); // chama a função de inserção interativa
			break;
        
		default: // se não for uma opção valida
			printf("Funcionalidade inválida.\n"); // retorna uma mensagem de erro para avisar que a funcionalidade escolhida é invalida 
			break;
    }

	return 0; // retorna zero pois o programa rodou como o esperado
}