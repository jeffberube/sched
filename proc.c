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

		/* Block new process */	
		kill(pid, SIGSTOP);

		/* Create new process node */
		pnode *proc = pnode_create(pid, name);
		
		/* Add process to circular linked list */
		pnode_add_ready(proc);

	}

	return 0;
}

/*
 * exec_process
 *
 * Executes a process within working directory
 *
 */

void exec_process(char filename[32]) {

	pid = 0;
	pid = fork();

	/* If child process */
	if (!pid) {

		/* Close read end of pipe on child */
		close(fd[0]);

		/* Redirect stdout to write end of pipe */
		dup2(fd[1], STDOUT_FILENO);
		dup2(fd[1], STDERR_FILENO);

		/* Close write end of pipe */
		close(fd[1]);

		/* Exec and test for error */
		execl(filename, filename, (char *) 0);

		/* If code reaches this point, exec failed, print error and flush */
		printf("ERROR: Could not execute process.\n");

		_exit(-1);
	
	} else {
		
		/* Block new process */
		kill(pid, SIGSTOP);

		/* Create process node */
		pnode *proc = pnode_create(pid, filename);

		/* Add process to circular linked list */
		pnode_add_ready(proc);
		
	}

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

	pnode *proc = pnode_get_node_by_pid(pid);

	/* If process was found */
	if (proc) {
		
		/* If process isn't blocked */
		if (proc->state != BLOCKED) {
			
			int head_is_tail = head == tail ? 1 : 0;
			int head_is_proc = head == proc ? 1 : 0;

			/* Stop process */
			kill(proc->pid, SIGSTOP);

			/* Remove from ready queue and put in blocked queue */
			pnode_remove_ready(proc);
			pnode_add_blocked(proc);

			/* More than one process in ready queue and blocking head */
			if (!head_is_tail) {
			
				if (head_is_proc) {
					
					kill(head->pid, SIGCONT);
					alarm(3);

				}

			/* Only one process in list, start idle */
			} else {

				kill(idle_proc->pid, SIGCONT);
				alarm(3);
			
			}

		/* If process is already blocked */
		} else 
			sprintf(errstr, "Process %d is already blocked.", pid);

	/* Process was not found */
	} else 
		sprintf(errstr, "ERROR: Process %d not found.", pid); 

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

	pnode *proc = pnode_get_node_by_pid(pid);

	/* If process was found */
	if (proc) {
	
		/* If process isn't ready */
		if (proc->state != READY) {

			pnode_remove_blocked(proc);
			pnode_add_ready(proc);

		/* Process already runnable */
		} else
			sprintf(errstr, "Process %d is already runnable.", pid);

	/* Process was not found */
	} else
		sprintf(errstr, "ERROR: Process %d not found.", pid);

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

		/* Store value before removing node from queue */
		int head_is_tail = head == tail ? 1 : 0;
		int head_is_tmp = head == tmp ? 1 : 0;

		/* Remove node from ready queue */
		pnode_remove_ready(tmp);
  
		/* Kill process */
		kill(tmp->pid, SIGKILL);

		/* More than one ready process and head is process to kill */
		if (!head_is_tail) {
		
			/* If head is process to be killed */
			if (head_is_tmp) {

				/* Start next process in line and reset alarm */
				kill(head->pid, SIGCONT);
				alarm(3);

			}

		/* If there is only one node in the list */
		} else {
			
			/* Start idle process and reset alarm */
			kill(idle_proc->pid, SIGCONT);
			alarm(3);

		}
			
		/* Destroy node */
		pnode_destroy(tmp);

	/* If process not found, display error message */
	} else
		sprintf(errstr, "ERROR: Process %d not found.", pid);

}


