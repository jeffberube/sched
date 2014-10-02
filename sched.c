/* 
 * @Author: 	Jeff Berube
 * @Title:	Scheduler (Assignment #1 - CS3790) 
 *
 * @Description: Scheduler simulator. Spawn and kill processes on the fly. Scheduling
 * 		uses a circular linked list like a real kernel and has an idle process
 * 		if no other processes are scheduled to run.
 *
 *	 	To use a command, type the command in the specified format and then 
 *	 	press enter at the "COMMAND > " prompt.
 *
 * @Commands:	
 *
 * 	spawn <processname>	Forks scheduler and spawns a process that outputs
 * 				processname to the pipe. Process name is 8 characters
 * 				maximum.
 *
 * 	kill <pid>		Kills a process using its process id (pid).	
 *
 * 	exec <filename>		Executes process within scheduler directory and pipes
 * 				the output to the scheduler.
 *
 * 	help			Displays a window with available commands and their
 * 				syntax.
 *
 *	quit			Quits the scheduler. Return to shell.
 *
 */

#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "pnode.h"
#include "ui.h"
#include "proc.h"

#define SPAWN		1
#define EXEC		2
#define KILL		3
#define HELP		4
#define QUIT		5

typedef enum status {on, off} status;

int pid, running_pid, fd[2];
status flag = on;
bool alarm_set = false;

/* Signal handling variables */
struct sigaction newhandler, oldhandler;
sigset_t sig;

/* Process table variables */
pnode *head, *tail, *idle_proc;

/* Terminal geometry variables. Updated in init_ncurses() */
int ncols = 80, nrows = 24;

/* Command buffer */
char comm[64] = {0};
int comm_ptr = 0;

/* Error line buffer */
char *errstr;

/*
 * log_add_line
 *
 * Adds a line to the log table
 *
 */

char *logtable[128];
int lastline = 0, logcount = 0;

void log_add_line(char *buffer) {

	char* message = malloc(sizeof(char) * sizeof(buffer));

	memset(message, '\0', sizeof(message));
	strcpy(message, buffer);

	logtable[lastline] = message;

	lastline++;

	lastline = lastline % (nrows - HEADER - FOOTER);

	if (logcount <= (nrows - HEADER - FOOTER) - 1) logcount++;

}

/* 
 * next
 *
 * Runs next process in process table. If no processes are queued, run idle process. 
 *
 * Arguments:
 *
 * (int)code: 	Contains signal code
 *
 */

void next(int code) {

	if(kill(running_pid, SIGSTOP) == -1)
		printf("%d: %s.\n", running_pid, strerror(errno));
	
	/* If there's a process in the list */
	if (head) { 

		/* Fix pointers */
		tail = head;
		head = head->next;

		/* Run next process */
		kill(head->pid, SIGCONT);

		running_pid = head->pid;

	/* If list is empty, run idle process */
	} else {

		kill(idle_proc->pid, SIGCONT);
		running_pid = idle_proc->pid;

	}

	alarm(3);

}

/*
 * setup_clock_int()
 *
 * Sets up mock clock interrupt handler
 *
 */

void setup_clock_int() {

	/* Setup signal handler struct */
	sigemptyset(&sig);
	newhandler.sa_handler = next;
	newhandler.sa_mask = sig;
	newhandler.sa_flags = 0;

	/* Try to install signal handler. On failure, exit. */
	if (sigaction(SIGALRM, &newhandler, &oldhandler) == -1) {
	
		printf("1 - Cannot install signal handler\n");
		exit(-1);

	}

	alarm(3);
}

/*
 * parse_command()
 *
 * Parses first word in string until it reaches a space character then
 * compares to list of valid commands.
 *
 * Returns a constant corresponding to the command code or -1 if an error occured
 *
 */

int validate_command(char *command) {

	char cb[64] = {0};
	char ch;

	int i = 0;

	/* Iterate through and put command in cb in lowercase */
	while ((ch = *(command + i)) != 0) {
		
		cb[i++] = tolower(ch);

	}

	if (strcmp(cb, "spawn")) return SPAWN;
	
	else if (strcmp(cb, "exec")) return EXEC;
	
	else if (strcmp(cb, "kill")) return KILL;
	
	else if (strcmp(cb, "help")) return HELP;
	
	else if (strcmp(cb, "quit")) return QUIT;
	
	else return -1;


}

/*
 * parse_param()
 *
 * Parses and validates parameter after command. 
 *
 * Returns 1 on success, 0 on error.
 *
 */

int validate_param(int c_code, char *param) {

	switch (c_code) {
	
		case SPAWN: ;
			
			/* Test string length. Max length is 8 characters */
			int i = 0;
			
			while (*(param + i) != 0) i++;
			
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
			if (sscanf(param, "%i", &arg_pid) != 1) {
			
				sprintf(errstr, "Error: \"%s\" is not a valid number.",
						param);

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

char **parse_command() {

	/* Variables to parse string into array */
	char **parsed = NULL;
	char *p = strtok(comm, " ");
	int spaces = 0, i;

	/* Split string in comm buffer and put in array parsed */
	while (p) {
		
		/* Resize parsed to accept new pointer */
		parsed = realloc(parsed, sizeof(char *) * ++spaces);

		/* If realloc failed, exit function and display error in log */
		if (parsed == NULL) {
			log_add_line("Could not parse command. Please try again.");
			return;
		}

		/* Add pointer to string to parsed */
		parsed[spaces - 1] = p;
		
		/* Keep reading from where last strtok call left off */
		p = strtok(NULL, " ");

	}

	/* Add final NULL pointer to find end of array */
	parsed = realloc(parsed, sizeof(char *) * ++spaces);
	parsed[spaces] = NULL;

	return parsed;

}

/*
 * exec_command()
 *
 * Tries to parse and execute the command sitting on the command line
 *
 */

void exec_command() {

	int c_code;
	char **args = parse_command();

	c_code = validate_command(args[0]);

	if (c_code != -1 && validate_param(c_code, args[1])) {

		switch (c_code) {
	
			case SPAWN:
				spawn_process(args[1]);
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
	
	free(args);	

	/* Reset command buffer and pointer */
	memset(comm, 0, sizeof(comm));
	comm_ptr = 0;	

	errstr = NULL;
}


/*
 * Main program
 *
 */

int main() {

	/* Init variables */
	char buffer[1024];

	/* Init pipe */
	pipe(fd);

	/* Fork monitor and idle process */
	pid = fork();

	/* If child process, this is the idle process */
	if (!pid) {
		
		char string[] = "Idle.\n";

		/* Close input side of pipe */
		close(fd[0]);
		
		while (1) {

			// Output string to pipe	
			write(fd[1], string, strlen(string) + 1);

			sleep(1);

		}
	
	/* Else this is parent process, this is scheduler */
	} else {

		/* Initiate gui */
		init_ncurses();

		/* Setup clock interrupt handler */
		setup_clock_int();

		/* Set flags for non blocking read call on pipe */
		int flags = fcntl(fd[0], F_GETFL, 0);
		fcntl(fd[0], F_SETFL, flags | O_NONBLOCK);

		/* Setup idle process */
		idle_proc = pnode_create(pid, "idle");
		
		running_pid = pid;

		int a = spawn_process("proc_a");
		a = spawn_process("proc_b");
		a = spawn_process("proc_c");

		next(0);	

		int ch, count;

		/* Enter main loop */
		while (1) {
			
			/* Poll to see if there is data to be read in the pipe */
			if((count = read(fd[0], buffer, 1024)) > 0) {
				char message[1024];
				strcpy(message, buffer);
				log_add_line(message);
			}
			 
			/* Poll to see if there is a character waiting in the buffer */
			ch = getch();

			switch (ch) {
			
				/* If no character in buffer */
				case ERR: 
					
					break;

				/* If character is backspace */
				case 7:
				case 127:
				case KEY_DC:
				case KEY_BACKSPACE:
					
					if (comm_ptr > 0) comm[comm_ptr - 1] = '\0';

					break;

				/* If character is enter, parse command and execute */
				case 10:
				case KEY_ENTER:
					if (comm_ptr > 0) exec_command();
					break;

				/* Else add character to buffer if buffer isnt full */
				default:
					if (comm_ptr < (sizeof(comm) - 1))
						comm[comm_ptr++] = (char)ch;
			
			} 
		
			/* Update screen */
			update_screen();

		} /* End main loop */
		
		/* Finish ncurses */
		end_ncurses();

	}

	return 0;
}
