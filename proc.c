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


