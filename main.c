#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>

#include "defines.h"


typedef enum{
	AMMINISTRATORE = 1,
	BIBLIOTECARIO,
	UTENTE,
	FAILED_LOGIN
} role_t;

struct configuration conf;

static MYSQL *conn;

static role_t attempt_login(MYSQL *conn, char *username, char *password){
	MYSQL_STMT *login_procedure;

	MYSQL_BIND param[3];
	int role = 0;

	if(!setup_prepared_stmt(&login_procedure, "call login(?,?,?)", conn)){
		print_stmt_error(login_procedure, "Unable to initialize login statement\n");
		goto err2;
	}

	//prepare parameters
	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_VAR_STRING; // IN
	param[0].buffer = username;
	param[0].buffer_length = strlen(username);

	param[1].buffer_type = MYSQL_TYPE_VAR_STRING; // IN
	param[1].buffer = password;
	param[1].buffer_length = strlen(password);

	param[2].buffer_type = MYSQL_TYPE_LONG; // OUT
	param[2].buffer = &role;
	param[2].buffer_length = sizeof(role);

	if (mysql_stmt_bind_param(login_procedure, param) != 0) { // Note _param
		print_stmt_error(login_procedure, "Could not bind parameters for login");
		goto err;
	}

	// Run procedure
	if(mysql_stmt_execute(login_procedure) != 0){
		print_stmt_error(login_procedure, "Could not execute login_procedure");
		goto err;
	}

	// Prepare output parameters
	memset(param, 0, sizeof(param));
	param[0].buffer_type = MYSQL_TYPE_LONG; // OUT
	param[0].buffer = &role;
	param[0].buffer_length = sizeof(role);

	if(mysql_stmt_bind_result(login_procedure, param)){
		print_stmt_error(login_procedure, "Could not performe login_procedure");
		goto err;
	}

	// Retrieve ouput paramenters
	if(mysql_stmt_fetch(login_procedure)){
		print_stmt_error(login_procedure, "Could not performe login_procedure");
		goto err;

	}

	mysql_stmt_close(login_procedure);
	return role;

	err:
	mysql_stmt_close(login_procedure);
	err2:
	return FAILED_LOGIN;
}

int main(void)
{
	role_t role;
	
	if(!parse_config("users/login.json", &conf)){
		fprintf(stderr, "Unable to load login configuration\n");
		exit(EXIT_FAILURE);
	}

	conn = mysql_init (NULL);
	if(conn == NULL){
		fprintf(stderr, "mysql_init() failed (probably out of memory)\n");
		exit(EXIT_FAILURE);
	}

	if(mysql_real_connect(conn, conf.host, conf.db_username, conf.db_password, conf.database, conf.port, NULL, CLIENT_MULTI_STATEMENTS | CLIENT_MULTI_RESULTS) == NULL) {
		fprintf(stderr, "mysql_real_connect() failed\n");
		mysql_close(conn);
		exit(EXIT_FAILURE);
	}

	printf("Username: ");
	getInput(128, conf.username, false);
	printf("Password: ");
	getInput(128, conf.password, true);

	role = attempt_login(conn, conf.username, conf.password);

	switch(role) {
		case AMMINISTRATORE:
			run_as_amministratore(conn);
			break;
			
		case BIBLIOTECARIO:
			run_as_bibliotecario(conn);
			break;
			
		case UTENTE:
			run_as_utente(conn);
			break;
			
		case FAILED_LOGIN:
			fprintf(stderr, "Invalid credentials\n");
			exit(EXIT_FAILURE);
			break;

		default:
			fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
			abort();
	}

	printf("Bye!\n");

	mysql_close (conn);
	return 0;


	return 0;
}