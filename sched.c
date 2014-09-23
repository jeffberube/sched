/* 
 * @Author: 	Jeff Berube
 * @Title:	Scheduler (Assignment #1) 
 *
 * @Description: Scheduler simulator. Spawn and kill processes on the fly.
 *
 */

#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>

#include "pnode.h"

typedef enum status {on, off} status;

unsigned short i = 0;
int pid, fd[2];
status flag = on;
bool alarm_set = false;

struct sigaction newhandler, oldhandler;
sigset_t sig;

pnode *head, *tail, *idle_proc;
char *logtable[32];

/* 
 * next
 *
 * Signals child process to block or continue. 
 *
 * Arguments:
 *
 * (int)code: 	Contains signal code
 *
 */

void next(int code) {

	static status flag = on;

	if (flag == on) {
	
		kill(pid, SIGSTOP);
		flag = off;

	} else {
		
		kill(pid, SIGCONT);
		flag = on;

	}

}

/*
 * spawn_process
 *
 * Spawns a new process in the scheduler. Adds process to list.
 *
 */

int spawn_process(int pid, char name[32]) {

	pid = fork();

	if (!pid) {
		
		char string[33];
		strcat(string, name);
		strcat(string, "\n");

		// Close input pipe on child process
		close(fd[0]);

		while (1) {

			sleep(1);
			write(fd[1], string, (strlen(string) + 1));
		
		}

	} else {
	
		// Create new process node
		pnode* proc = pnode_create(pid, name);
		
		// Add process to circular linked list
		if(!head) {
			
			head = tail = proc;
			proc->next = proc;
			proc->prev = proc;
		
		} else {
			
			tail->next = proc;
			proc->next = head;
			proc->prev = tail;
			tail = tail->next;
		}

	}

	return 0;
}

/*
 * kill_process
 *
 * Kills process from scheduling list.
 *
 */

void kill_process(int pid) {

	pnode *tmp = pnode_get_node_by_pid(pid);
	int tmppid = tmp->pid;

	if (tmp) {
		
		// If node to kill is head, make head next node and adjust tail pointer
		if (head == tmp) {
			head = tmp->next;
			if (head) head->prev = tail;
		}

		// If node to kill is tail, make tail previous and adjust head pointer
		if (tail == tmp) {
			tail = tail->prev;
			if (tail) tail->next = head;
		}
		
		if

		pnode_destroy(tmp);
		kill(tmppid, SIGKILL);

	} else {
	
		char error[] = "";
		log_add_line(error);

	}

}

/*
 * log_add_line
 *
 * Adds a line to the output window
 *
 */

void log_add_line(char *buffer) {

	static int lastline = 0;

	logtable[lastline] = buffer;

	lastline++;

	lastline = lastline % 32;

}

/*
 * update_screen
 *
 * Updates the current screen.
 *
 */

void update_screen() {

	int cols, rows;

	// Get terminal size
	getmaxyx(stdscr, rows, cols);

	// Print label "Output window"
	

}

/*
 * Main program
 *
 */

int main() {

	// Init variables
	char buffer[32];

	// Init pipe
	pipe(fd);

	// Fork monitor and idle process
	pid = fork();

	if (!pid) {
		
		char string[] = "Idle.\n";

		// Close input side of pipe
		close(fd[0]);

		// Main loop
		while (1) {

			sleep(1);
			// Output string to pipe	
			write(fd[1], string, (strlen(string) + 1));

		}
	
	} else {

		// Close output side of pipe
		close(fd[1]);

		// Setup handler struct
		sigemptyset(&sig);
		newhandler.sa_handler = next;
		newhandler.sa_mask = sig;
		newhandler.sa_flags = 0;

		// Try to install signal handler. On failure, exit.
		if (sigaction(SIGALRM, &newhandler, &oldhandler) == -1) {
		
			printf("1 - Cannot install signal handler\n");
			exit(-1);

		}
		
		// Setup idle process
		idle_proc = pnode_create(pid, "idle");


		// Init ncurses
		initscr();

		while(1) {
			
			// Keep reading from pipe
			read(fd[0], buffer, sizeof(buffer));
			log_add_line(buffer);
			
			// Update screen
			update_screen();
		}
		
		// Finish ncurses
		endwin();
	}

	return 0;
}
