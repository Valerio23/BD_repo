#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"

void run_as_bibliotecario(MYSQL *conn){

	char options[5] = {'1', '2', '3', '4', '5'};
	char op;

	printf("Switching to libriarian role...\n");

	if(!parse_config("users/bibliotecario.json", &conf)){
		fprintf(stderr, "Unable to load libriarian configuration\n");
		exit(EXIT_FAILURE);
	}

	if(mysql_change_user(conn, conf.db_username, conf.db_password, conf.database)){
		fprintf(stderr, "mysql_change_user() failed\n");
		exit(EXIT_FAILURE);
	}

	while(true){

		printf("\033[2J\033[H");
		printf("*** What should I do for you? ***\n\n");
		printf("1) Richiesta di malattia\n");
		printf("2) Disponibilità copie\n");
		printf("3) Richiesta di trasferimento\n");
		printf("4) Report libri\n");
		printf("5) Quit\n");

		op = multiChoice("Select an option", options, 5);

		switch(op) {
			case '1':
				//richiesta_di_malattia(conn);
				break;

			case '2':
				//disponibilita_copie(conn);
				break;

			case '3':
				//richiesta_di_trasferimento(conn);
				break;

			case '4':
				//report_libri(conn);
				break;
			
			case '5':
				return;
				
			default:
				fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
				abort();
		}

		getchar();

	}


}