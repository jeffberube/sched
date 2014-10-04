/*
 * @Author:	Jeff Berube
 * @Title:	comm.c
 *
 * @Description: Contains all the command processing functions
 *
 */

#include "comm.h"

/*
 * validate_command()
 *
 * Compares args[0] to list of valid commands.
 *
 * Returns a constant corresponding to the command code or -1 if an error occured
 *
 */

int validate_command() {

	char cb[64] = {0};
	int i = 0;

	/* Iterate through and put command in cb in lowercase */
	while (args[0][i] != 0) {

		cb[i] = tolower(args[0][i]);
		i++;
	
	}

	/* Return appropriate constant */
	if (!strcmp(cb, "spawn")) return SPAWN;
	
	else if (!strcmp(cb, "exec")) return EXEC;
	
	else if (!strcmp(cb, "kill")) return KILL;
	
	else if (!strcmp(cb, "help")) return HELP;
	
	else if (!strcmp(cb, "quit")) return QUIT;
	
	else return -1;


}

/*
 * validate_param()
 *
 * Validates parameter after command. 
 *
 * Returns 1 on success, 0 on error.
 *
 */

int validate_param() {

	switch (validate_command(args[0])) {
	
		case SPAWN: ;
			
			/* Test string length. Max length is 8 characters */
			int i = 0;
			
			while (args[1][i] != 0) i++;
			
			if (i <= 8) return 1;
			else {
			
				errstr = 
				"Error: Maximum process name length is 8 characters";
				
				return 0;	
			}
			
			break;

		case EXEC:

			break;

		case KILL: ;

			/* Parse string to int */
			int arg_pid;

			/* If cannot parse parameter to int */
			if (sscanf(args[1], "%i", &arg_pid) != 1) {
			
				sprintf(errstr, "Error: \"%s\" is not a valid number.",
						args[1]);

				return 0;

			/* If cannot find pid in process list */
			} else if (!pnode_get_node_by_pid(arg_pid)) {
						
				sprintf(errstr,	"Error: Could not find process %d.",
						arg_pid);
			
			/* Else success */
			} else return 1;

			break;
		
		default:
			return 1;
			break;

	}

}

/*
 * parse_command()
 *
 * Splits command string on spaces and returns pointer to array of strings
 *
 */

void parse_command() {

	/* Variables to parse string into array */
	int spaces = 0, i = 0, j = 0;

	/* Remove leading white space until first alpha character */
	while (i < sizeof(comm) && comm[i] == ' ' && comm[i] != 0) i++;
	
	/* Store command */
	while (i < sizeof(comm) && comm[i] != ' ' && comm[i] != 0) {
		
		args[0][j++] = comm[i];
		i++;

	}

	/* Remove white space between command and argument */
	while (i < sizeof(comm) && comm[i] == ' ' && comm[i] != 0) i++;

	j = 0;

	/* Store argument */
	while (i < sizeof(comm) && comm[i] != ' ' && comm[i] != 0) {
	
		args[1][j++] = comm[i];
		i++;

	}	

}

/*
 * exec_command()
 *
 * Tries to parse and execute the command sitting on the command line
 *
 */

void exec_command() {

	int c_code;
	char *arg1;
	
	/* Reset error string on new command */
	errstr = NULL;

	parse_command();
	c_code = validate_command();

	errstr = (char *)realloc(errstr, strlen(args[0]) + 1);
	sprintf(errstr, "%i", c_code);

	if (c_code != -1 && validate_param()) {

		switch (c_code) {
	
			case SPAWN:
				/* Copy argument into pointer */
				arg1 = malloc(strlen(args[1]));
			        strcpy(arg1, args[1]);	

				/* Spawn new process */
				spawn_process(arg1);
				break;

			case EXEC:
				exec_process(args[1]);
				break;

			case KILL: ;
				int arg_pid;
				sprintf(args[1], "%i", &arg_pid);
				kill_process(arg_pid);
				break;

			case HELP:
				show_help();
				break;

			case QUIT:
				end_ncurses();
				exit(0);
				break;
	
		}

	} else if (c_code == -1) {
	
		sprintf(errstr, "Error: \"%s\" is not a valid command.", args[0]);	

	} 
	
	/* Reset command buffer and pointer */
	memset(comm, 0, sizeof(comm));
	memset(args[0], 0, sizeof(args[0]));
	memset(args[1], 0, sizeof(args[1]));
	comm_ptr = 0;

}



