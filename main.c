// Dupla 31
// Aluno	: Daniel Dias Silva Filho ------------ N� USP: 13677114
// Aluno : Kaue Ant�nio Maranzano de Ara�jo --- N� USP: 14669684

#include "estruturas.h"

int main() {

	int funcionalidade; // declara uma variavel para salvar a escolha do usu�rio
	char arg1[100], arg2[100], arg3[100]; // declara 3 poss�veis argumentos para o usu�rio passar ap�s a escolha

	scanf("%d", &funcionalidade); // recebe a escolha do usu�rio pelo STDin

	switch (funcionalidade) {

		case 1: // se a funcionalidade escolhida for a numero 1:
			scanf("%s", arg1);	// recebe o nome do arquivo de �ndice primario a ser criado
			CREAT_INDEX(arg1);	// chama a fun��o de criar arquivo de indice primario passando o unico argumento necessario
			break;

		case 2: // se a funcionalidade escolhida for a numero 2:
			scanf("%s", arg1); // recebe o nome do arquivo csv de entrada
			scanf("%s", arg2); // recebe o nome do arquivo bin�rio de saida
			scanf("%s", arg3); // recebe o nome do arquivo de �ndice prim�rio
			CREAT_TABLE(arg1, arg2, arg3); // chama a fun��o de criar uma tabela passando os 3 argumentos necessarios
			break;

		case 3: // se a funcionalidade escolhida for a numero 3 :
			scanf("%s", arg1); // recebe o nome do arquivo bin�rio a ser listado
			SELECT(arg1); // chama a fun��o de listar tabela passando o �nico argumento necess�rio
			break;

		case 4: // se a funcionalidade escolhida for a numero 4:
			scanf("%s", arg1); // recebe o nome do arquivo bin�rio de entrada
			scanf("%s", arg2); // recebe o nome do arquivo bin�rio de �ndice primario
			scanf("%s", arg3); // recebe a quantidade de vezes que a busca deve ser feita
			SELECT_WHERE(arg1, arg2, atoi(arg3)); // chama a fun��o de buscar e listar pessoas se baseando em crit�rios espec�ficos
			break;
    }

	return 0; // retorna zero pois o programa rodou como o esperado
}
