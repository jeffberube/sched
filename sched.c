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
#include "comm.h"

int pid, running_pid, fd[2];

/* Signal handling variables */
struct sigaction newhandler, oldhandler, resizehandler;
sigset_t sig;

/* Process table variables */
pnode *head, *tail, *blocked, *idle_proc;

/* Terminal geometry variables. Updated in init_ncurses() */
int ncols = 80, nrows = 24;

/* Command buffer */
char comm[64] = {0};
int comm_ptr = 0;
char args[2][64] = {0};

/* Command history */
char *history[HIST_MAX];
int hist_ptr = 0, hist_count;

/* Error line buffer */
char errstr[128] = {0};

/* Output log table */
char *logtable[128];
int lastline = 0, logcount = 0;

/*
 * winch_handler
 *
 * Handles terminal resize
 *
 */

void winch_handler(int code) {

	endwin();
	refresh();
	clear();

	getmaxyx(stdscr, nrows, ncols);
}

/* 
 * next
 *
 * Runs next process in process table. If no processes are queued, run idle process. 
 *
 * NB: code argument is passed by OS on signal
 *
 */

void next(int code) {

	/* Stop currently running process */
	kill(running_pid, SIGSTOP); 
	
	/* If there's a process in the list */
	if (head) { 

		/* Fix pointers */
		tail = head;
		head = head->next;

		/* If there is a process left in the list */
		if (head) {
			
			kill(head->pid, SIGCONT);
			running_pid = head->pid;

		/* Else run idle */
		} else if (!kill(idle_proc->pid, 0)) {

			kill(idle_proc->pid, SIGCONT);
			running_pid = idle_proc->pid;
		
		/* If idle is not alive, fail catastrophically */
		} else 
			exit(-1);

	/* If list is empty, run idle process */
	} else if (!kill(idle_proc->pid, 0)) {

		kill(idle_proc->pid, SIGCONT);
		running_pid = idle_proc->pid;

	/* If idle is not alive, fail catastrophically */
	} else
		exit(-1);

	alarm(3);

}

/*
 * setup_winch_handler
 *
 * Sets up signal handler for terminal resize
 *
 */

void setup_winch_handler() {

	memset(&resizehandler, 0, sizeof(struct sigaction));
	resizehandler.sa_handler = winch_handler;
	sigaction(SIGWINCH, &resizehandler, NULL);

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

		/* Initiate gui and setup signal handler for terminal resize*/
		init_ncurses();
		setup_winch_handler();

		/* Setup clock interrupt handler */
		setup_clock_int();

		/* Set flags for non blocking read call on pipe */
		int flags = fcntl(fd[0], F_GETFL, 0);
		fcntl(fd[0], F_SETFL, flags | O_NONBLOCK);

		/* Setup idle process */
		idle_proc = pnode_create(pid, "idle");
		
		running_pid = pid;

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
			 
			keypad(stdscr, true);

			/* Poll to see if there is a character waiting in the buffer */
			ch = getch();

			switch (ch) {
			
				/* If no character in buffer */
				case ERR: 
				case 255:		
				case KEY_LEFT:
				case KEY_RIGHT:

					break;

				/* On key up, show previous command in history */
				case KEY_UP:

					history_get_prev();
					break;

				/* On key down, show next command in history */
				case KEY_DOWN:

					history_get_next();
					break;

				/* If character is backspace */
				case 7:
				//case 127:
				//case KEY_DC:
				case KEY_BACKSPACE:
		
					if (comm_ptr > 0) comm[--comm_ptr] = '\0';
					break;

				/* If character is enter, parse command and execute */
				case '\n':
				case KEY_ENTER:
					if (strlen(comm)) exec_command();
					break;

				/* Else add character to buffer if buffer isnt full */
				default:
					if (comm_ptr < (sizeof(comm) - 1) &&
							ch >= 32 && ch <= 126)
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
