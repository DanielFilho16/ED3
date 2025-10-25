// Dupla 31
// Aluno: Daniel Dias Silva Filho ------------ N USP: 13677114
// Aluno: Kaue Antonio Maranzano de Araujo --- N USP: 14669684

#include "estruturas.h"
#include "busca.h"

int main() {

	int funcionalidade; 
	char arg1[100], arg2[100], arg3[100]; 

	scanf("%d", &funcionalidade); 

	switch (funcionalidade) {

		case 1: 
			scanf("%s", arg1);	
			CREAT_INDEX(arg1);	
			break;

		case 2: 
			scanf("%s", arg1); 
			scanf("%s", arg2); 
			scanf("%s", arg3); 
			CREAT_TABLE(arg1, arg2, arg3); 
			break;

		case 3: 
			scanf("%s", arg1); 
			SELECT(arg1); 
			break;

		case 4: 
			scanf("%s", arg1); 
			scanf("%s", arg2); 
			scanf("%s", arg3); 
			SELECT_WHERE(arg1, arg2, atoi(arg3)); 
			break;

		case 5: 
			scanf("%s", arg1); 
			scanf("%s", arg2); 
			DELETE_FROM(arg1, arg2); 
			break;

		case 6: 
			scanf("%s", arg1); 
			scanf("%s", arg2); 
			INSERT_INTO(arg1, arg2); 
			break;

		case 7: 
			scanf("%s", arg1); 
			scanf("%s", arg2); 
			UPDATE(arg1, arg2); 
			break;

		case 8: 
			scanf("%s", arg1); 
			scanf("%s", arg2); 
			CREAT_TABLE_SEGUE(arg1, arg2); 
			break;

		case 9: 
			scanf("%s", arg1); 
			scanf("%s", arg2); 
			ORDER_BY(arg1, arg2); 
			break;

		case 10: 
			scanf("%s", arg1); 
			scanf("%s", arg2); 
			scanf("%s", arg3); 
			JOIN(arg1, arg2, arg3, 1); 
			break;

		default: 
			printf("Funcionalidade inv�lida.\n"); 
			break;
    }

	return 0; 
}
