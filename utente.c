#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"

static void iscrizione_utente(MYSQL *conn){

	

}

void run_as_utente(MYSQL *conn){

	char options[3] = {'1', '2', '3'};
	char op;

	printf("Switching to user role...\n");
	if(!parse_config("users/utente.json", &conf)){
		fprintf(stderr, "Unable to load user configuration\n");
		exit(EXIT_FAILURE);
	}

	if(mysql_change_user(conn, conf.db_username, conf.db_password, conf.database)){
		fprintf(stderr, "mysql_change_user() failed\n");
		exit(EXIT_FAILURE);
	}

	while(true){

		printf("\033[2J\033[H");
		printf("*** What should I do for you? ***\n\n");
		printf("1) Richiesta di prestito\n");
		printf("2) Restituzione prestito\n");
		printf("3) Quit\n");

		op = multiChoice("Select an option", options, 4);

		switch(op) {
			case '1':
				//richiesta_di_prestito(conn);
				break;
				
			case '2':
				//restituzione_prestito(conn);
				break;

			case '3':
				return;
				
			default:
				fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
				abort();
		}

		getchar();

	}
}