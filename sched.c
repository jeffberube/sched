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
#include <errno.h>

#include "pnode.h"

#define PADDING 1
#define HEADER	3	/* How many lines the header spans */
#define FOOTER	3	/* How many lines the footer spans */

typedef enum status {on, off} status;

int pid, running_pid, fd[2];
status flag = on;
bool alarm_set = false;

/*
 * Signal handling variables
 *
 */

struct sigaction newhandler, oldhandler;
sigset_t sig;

pnode *head, *tail, *idle_proc;

/*
 * Set global variables of terminal geometry to defaults.
 * Gets updated when initializing ncurses or when terminal window is resized.
 *
 */
 
int ncols = 80, nrows = 24;

char comm[1024];
int comm_pos = 0;

/*
 * log_add_line
 *
 * Adds a line to the output window
 *
 */

char *logtable[128];
int lastline = 0, logcount = 0;

void log_add_line(char *buffer) {

	logtable[lastline] = buffer;

	lastline++;

	lastline = lastline % (nrows - HEADER - FOOTER);

	if (logcount < (nrows - HEADER - FOOTER) - 1) logcount++;
}

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

	kill(running_pid, SIGSTOP);

	/* If there's a process in the list */
	if (head) { 

		/* Run next process */
		kill(head->pid, SIGCONT);

		/* Fix pointers */
		tail = head;
		head = head->next;

	/* If list is empty, run idle process */
	} else {

		kill(idle_proc->pid, SIGCONT);
	
	}

	alarm(3);

}

/*
 * spawn_process
 *
 * Spawns a new process in the scheduler. Adds process to list.
 *
 */

int spawn_process(char name[32]) {

	pid = 0;
	pid = fork();

	/* If child process */
	if (!pid) {
		
		char string[33] = "";
		strcat(string, name);
		strcat(string, "\n");

		/* Close input pipe on child process */
		close(fd[0]);

		while (1) {

			sleep(1);

			if(write(fd[1], string, strlen(string) + 1) != strlen(string) + 1)
			printf("%s", strerror(errno));
			printf("%s", string);	
		}

	/* If parent process */
	} else {
	
		kill(pid, SIGSTOP);

		/* Create new process node */
		pnode* proc = pnode_create(pid, name);
		
		/* Add process to circular linked list */
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

	/* If process is found, adjust list and destroy process */
	if (tmp) {
		
		if (head != tail) {
		
			/*
			 * If head is node to kill, 
			 * point head to next node and adjust tail pointer
			 *
			 */

			if (head == tmp) {
				head = tmp->next;
				if (head) head->prev = tail;
			}

			/*
			 *  If node to kill is tail, 
			 *  point tail to previous node and adjust head pointer
			 *
			 */

			if (tail == tmp) {
				tail = tail->prev;
				if (tail) tail->next = head;
			}
		
		} else head = tail = NULL;
	
		/* Destroy node */
		pnode_destroy(tmp);
		kill(tmppid, SIGKILL);

		/* Run next process in line. If list is empty, run idle process. */
		if (head) kill(head->pid, SIGCONT);
		else kill(idle_proc->pid, SIGCONT);

	/* If process not found, display error message */
	} else {
	
		char error[] = "";
		log_add_line(error);

	}

}

/*
 * ani_char()
 *
 * Returns index of spinning character
 *
 */

int ani_char() {

	static int i = 0, ch = 0;

	i++;
	i = i % 1000;

	if (!i) {
		ch++;
		ch = ch % 4;
	}

	return ch;

}

/*
 * update_screen
 *
 * Updates the current screen.
 *
 */


void update_screen() {

	char ani[] = {'/', '-', '\\', '|', 0};

	erase();

	/* Print label "Piped Output:" */
	mvprintw(PADDING + 0, PADDING + 1, "Piped Output: \n");	

	/* Print label "Process Table:" */
	mvprintw(PADDING + 0, ncols * 0.6 - PADDING, "Process Table: \n");

	/* Print process table column labels */
	mvprintw(PADDING + 1, ncols * 0.6 - PADDING, "PID\t\tName");

	/* Print log under Piped Output */
	int j = 0;
	int logrows = nrows - HEADER - FOOTER;
	int startline = logcount < logrows ? 0 : lastline;

	
	while (j < logrows) {
		
		int line = (startline + j) % logrows;
		
		mvprintw(PADDING + 23, PADDING + 1, 
				"logrows:%d startline:%d lastline:%d line:%d j:%d", 
				logrows, startline, lastline, line, j);
		
		if(logtable[line]) 
			mvprintw(PADDING + 2 + j, PADDING + 1, "%s", logtable[line]);
		
		j++;

	}

	mvprintw(nrows - 1, PADDING + 1, "%d", j);

	/* Print process table */
	mvprintw(PADDING + 2, ncols * 0.6 - PADDING - 2, "%c", ani[ani_char()]);
	
	/* Maintains which row to print to */
	int i = 0;

	/* If there is a process in the list */
	if (head) {
		
		mvprintw(PADDING + 2 + i, ncols * 0.6 - PADDING, "%d\t%s", 
				head->pid, head->name);	

		i++;

		/* If there is more than one process in the list */
		if (head->next && head != tail) {
		
			pnode *tmp = head;

			while (tmp->next) {
			
				tmp = tmp->next;
				mvprintw(PADDING + 2 + i, ncols * 0.6 - PADDING, 
						"%d\t%s", tmp->name);

				i++;
			}
		
		}

	}

	/* Print idle process */
	attron(COLOR_PAIR(3));
	mvprintw(PADDING + 2 + i, ncols * 0.6 - PADDING, "%d\tIdle\n", idle_proc->pid);
	attroff(COLOR_PAIR(3));

	refresh();

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

	/* If child process */
	if (!pid) {
		
		char string[] = "Idle.\n";

		/* Close input side of pipe */
		close(fd[0]);
		
		while (1) {

			sleep(1);
			// Output string to pipe	
			write(fd[1], string, strlen(string) + 1);
		}
	
	/* If parent process */
	} else {

		/* Close output side of pipe */
		close(fd[1]);

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
		
		/* Set flags for non blocking read call */
		int flags = fcntl(fd[0], F_GETFL, 0);
		fcntl(fd[0], F_SETFL, flags | O_NONBLOCK);

		/* Setup idle process */
		idle_proc = pnode_create(pid, "idle");
		
		running_pid = pid;

		int a = spawn_process("proc_a");
		//a = spawn_process("proc_b");
		//a = spawn_process("proc_c");
	
		/* Init ncurses and set parameters */
		initscr();
		noecho();
		halfdelay(1); 
		getmaxyx(stdscr, nrows, ncols); 
		
		/* Setup ncurses colors */
		start_color(); 
		
		/* If terminal can change colors, change cyan to light gray */
		if(can_change_color()) 
			init_color(COLOR_CYAN, 600, 600, 600); 
		
		init_pair(1, COLOR_WHITE, COLOR_BLACK);
		init_pair(2, COLOR_RED, COLOR_BLACK);
		init_pair(3, COLOR_CYAN, COLOR_BLACK);
		
		char ch;
		int test = 0, count;

		alarm(3);

		while(1) {
			
			/* Poll to see if there is data to be read */
			if((count = read(fd[0], buffer, 1024)) > 0) {
				char message[1024];
				strcpy(message, buffer);
				log_add_line(message);
			}
			 
			/* Keep reading command line
			ch = getch();
			if (ch != ERR) {
				comm[comm_pos++] = ch;
			} 
			*/
			
			/* Update screen */
			update_screen();
		}
		
		/* Finish ncurses */
		endwin();

	}

	return 0;
}
