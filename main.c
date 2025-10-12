// Dupla 31
// Aluno: Daniel Dias Silva Filho ------------ N USP: 13677114
// Aluno: Kaue Antonio Maranzano de Araujo --- N USP: 14669684

#include "estruturas.h"

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

		default: 
			printf("Funcionalidade inv�lida.\n"); 
			break;
    }

	return 0; 
}
