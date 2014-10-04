/*
 * @Author:	Jeff Berube
 * @Title:	proc
 *
 * @Description: Contains process related functions
 *
 */

#include "proc.h"

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

		/* Close read end of pipe on child process */
		close(fd[0]);

		while (1) {

			if(write(fd[1], string, strlen(string) + 1) != strlen(string) + 1)
				printf("\n%s", strerror(errno));

			sleep(1);
		}

	/* If parent process */
	} else {
	
		kill(pid, SIGSTOP);

		/* Create new process node */
		pnode* proc = pnode_create(pid, name);
		
		/* Add process to circular linked list */
		
		/* If list is empty */
		if(!head) {
		
			/* Adjust pointers */
			head = tail = proc;
			proc->next = proc;
			proc->prev = proc;
			
			/* Block idle process, start new process and reset alarm*/
			kill(idle_proc->pid, SIGSTOP);
			kill(proc->pid, SIGCONT);
			alarm(3);

		/* If list is not empty */
		} else {
	
			tail->next = proc;
			proc->next = head;
			proc->prev = tail;
			tail = proc;
			head->prev = tail;
			
		}


	}

	return 0;
}

/*
 * exec_process
 *
 * Executes a process within working directory
 *
 */

void exec_process(char *filename) {



}

/*
 * block_process
 *
 * Sets process state to BLOCKED. Blocks process if currently running.
 *
 * Takes process id as argument.
 *
 */

void block_process(int pid) {



}

/*
 * run_process
 *
 * Sets process state to READY. Puts process at end of ready queue.
 *
 * Takes process id as argument.
 *
 */

void run_process(int pid) {



}

/*
 * kill_process
 *
 * Kills process from scheduling list. Takes process id as argument.
 *
 */

void kill_process(int pid) {

	pnode *tmp = pnode_get_node_by_pid(pid);

	/* If process is found, adjust list and destroy process */
	if (tmp) {
		
		/* Kill process */
		kill(tmp->pid, SIGKILL);

		/* If there is more than one node in the list */
		if (head != tail) {
		
			/* If head is the node to kill */
			if (head == tmp) {
				
				/* Adjust head and tail pointers */
				head = head->next;
				tail->next = head;
				head->prev = tail;

				/* In case there is only 2 node in the list */
				if (head->next == tmp) head->next = head;

				/* Start next process in line and reset alarm */
				kill(head->pid, SIGCONT);
				alarm(3);
				
			/* If tail is the node to kill */
			} else if (tail == tmp) {
				
				/* Adjust head and tail pointer */
				tail = tail->prev;
				head->prev = tail;
				tail->next = head;

				if (tail->prev == tmp) tail->prev = tail;
			} else 
				tmp->prev->next = tmp->next;

		/* If there is only one node in the list */
		} else {
			
			head = tail = NULL;

			/* Start idle process and reset alarm */
			kill(idle_proc->pid, SIGCONT);
			alarm(3);

		}
			
		/* Destroy node */
		pnode_destroy(tmp);

	/* If process not found, display error message */
	} else {
	
		sprintf(errstr, "ERROR: Process %d not found.", pid);

	}

}


