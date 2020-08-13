#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"



static void add_new_user(MYSQL *conn){
	MYSQL_STMT *prepared_stmt;
	MYSQL_BIND param[6];

	// Input for the registration routine
	char name[31];
	char surname[31];
	MYSQL_TIME  date_birth;
	char sex[6];
	char username[46];
	int id;

	// Get the required information
	printf("\nUser name: ");
	getInput(31, name, false);
	printf("\nUser surname: ");
	getInput(31, surname, false);
	printf("\nUser birth date: ");
	...
	printf("\nUser sex: ");
	getInput(6, sex, false);
	printf("\nUser username: ");
	getInput(46, username, false);

	// Prepare prepared statement
	if(!setup_prepared_stmt(&prepared_stmt, "call aggiungi_user(?,?,?,?,?,?)", conn)){
		finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize user insertion statement\n", false);
	}

	// Prepare parameters
	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = name;
	param[0].buffer_length = strlen(name);

	param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[1].buffer = surname;
	param[1].buffer_length = strlen(surname);

	param[2].buffer_type = MYSQL_TYPE_DATE;
	param[2].buffer = date_birth;
	param[2].buffer_length = strlen(birth);

	param[3].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[3].buffer = sex;
	param[3].buffer_length = strlen(sex);

	param[4].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[4].buffer = username;
	param[4].buffer_length = strlen(username);

	param[5].buffer_type = MYSQL_TYPE_LONG;
	param[5].buffer = &id;
	param[5].buffer_length = sizeof(id);

	if(mysql_stmt_bind_param(prepared_stmt, param) != 0){
		finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for user inseriton\n", true);
	}

	// Run procedure
	if(mysql_stmt_execute(prepared_stmt) != 0){
		print_stmt_error(prepared_stmt, "An error occurred while adding the user");
		goto out;
	}

	// Get back the ID of the newly_added user
	memset(param, 0, sizeof(param));
	param[0].buffer_type = MYSQL_TYPE_LONG;
	param[0].buffer = &id;
	param[0].buffer_length = strlen(id);

	if(mysql_stmt_bind_result(prepared_stmt, param)){
		finish_with_stmt_error(conn, prepared_stmt, "Could not retrieve output parameter", true);
	}

	// Retrieve output parameter
	if(mysql_stmt_fetch(prepared_stmt)){
		finish_with_stmt_error(conn, prepared_stmt, "Could not buffe the results", true);

	}

	printf("User correctly added with ID: %d\n", id);

	out:
	mysql_stmt_close(prepared_stmt);

}

static void subscrive_user_to_a_library(MYSQL *conn){

	MYQL_STMT *prepared_stmt;
	MYSQL_BIND param[2];

	// Input for registration routine
	int id_user;
	int id_biblioteca;

	// Ger required information
	printf("\nUser id: ");
	scanf("%d", &id_user);
	printf("\nLibrary id: ");
	scanf("%d", &id_biblioteca);

	// Prepare stored procedure
	if(!setup_prepared_stmt(&prepared_stmt, "call iscrivi_user(?, ?)", conn)){
		finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize user registration statement\n", false);
	}

	// Prepare parameters
	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_LONG;
	param[0].buffer = &id_user;
	param[0].buffer_length = sizeof(id_user);

	param[1].buffer_type = MYSQL_TYPE_LONG;
	param[1].buffer = &id_biblioteca;
	param[1].buffer_length = sizeof(id_biblioteca);

	if(mysql_stmt_bind_param(prepared_stmt, param) != 0){
		finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for user registration\n", true);
	}

	// Run procedure
	if(mysql_stmt_execute(prepared_stmt) != 0){
		print_stmt_error(prepared_stmt, "An error occured while registring the user");
	}else{
		printf("User correctly registred\n");
	}

	mysql_stmt_close(prepared_stmt);


}

void run_as_amministratore(MYSQL *conn){

	char options[7] = {'1', '2', '3', '4', '5', '6', '7'};
	char op;

	printf("Switching the administrative role...\n");

	if(!parse_config("users/amministratore.json", &conf)){
		fprintf(stderr, "Unable to load administrator configuration\n");
		exit(EXIT_FAILURE);

	}

	if(mysql_change_user(conn, conf.db_username, conf.db_password, conf.database)){
		fprintf(stderr, "mysql_change_user failed\n");
		exit(EXIT_FAILURE);
	}

	while(true){

		printf("\033[2J\033[H");
		printf("*** What should I do for you? ***\n\n");
		printf("1) Add new user");
		printf("2) Subscribe user to a library\n");
		printf("3) Create work slots\n");
		printf("4) Find slots substitutions\n");
		printf("5) Dismiss book\n");
		printf("6) Report slots\n");
		printf("7) Quit\n");

		op = multiChoice("Select an option", options, 5);

		switch(op){

			case '1':
				add_new_user(conn);
				break;
			case '2':
				subscrive_user_to_a_library(conn);
				break;
			case '3':
				//create_work_slot(conn);
				break;
			case '4':
				//find_slots_substitutions(conn);
				break;
			case '5':
				//dismiss_book(conn);
				break;
			case '6':
				//report_slots(conn);
				break;
			case '7':
				return;
		
			default:
				fprintf(stderr, "Invalid condition at %s: %d\n", __FILE__, __LINE__);
				abort();
		}

		getchar();
	}
}