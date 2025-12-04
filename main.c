// Dupla 31
// Aluno: Daniel Dias Silva Filho ------------ N USP: 13677114
// Aluno: Kaue Antonio Maranzano de Araujo --- N USP: 14669684

#include "estruturas.h"
#include "busca.h"
#include "crud_pessoa.h"
#include "crud_segue.h"
#include "grafo.h"
#include "utils.h"

int main() {

	int funcionalidade; 
	char arg1[100], arg2[100], arg3[100], arg4[100]; 

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
			scanf("%s", arg4);
			JOIN(arg1, arg2, arg3, atoi(arg4)); 
			break;

		case 11:
			scanf("%s", arg1);
			scanf("%s", arg2);
			scanf("%s", arg3);
			funcionalidade11(arg1, arg2, arg3);
			break;

		case 12:
			scanf("%s", arg1);
			scanf("%s", arg2);
			scanf("%s", arg3);
			funcionalidade12(arg1, arg2, arg3);
			break;

		case 13:
			scanf("%s", arg1);
			scanf("%s", arg2);
			scanf("%s", arg3);
			scan_quote_string(arg4);
			funcionalidade13(arg1, arg2, arg3, arg4);
			break;

		case 14:
			scanf("%s", arg1);
			scanf("%s", arg2);
			scanf("%s", arg3);
			scan_quote_string(arg4);
			funcionalidade14(arg1, arg2, arg3, arg4);
			break;

		default: 
			printf("Funcionalidade inválida.\n"); 
			break;
    }

	return 0; 
}
