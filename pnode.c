/*
 * @Author:	Jeff Berube
 * @Title:	pnode (Doubly linked node for process scheduler)
 *
 */

#include "pnode.h"

/*
 * pnode_create
 *
 * Creates a new pnode and returns a pointer to the newly created pnode;
 *
 */

pnode* pnode_create(int pid, char *name) {
	
	/* Allocate node and set pid */
	pnode *node = malloc(sizeof(pnode));
	node->pid = pid;
	
	/* Copy name into node */
	node->name = malloc(strlen(name) + 1);
	strcpy(node->name, name);

	/* Set node state */	
	node->state = READY;

	return node;

}

/*
 * pnode_get_node_by_pid
 *
 * Iterates through processes and returns process with corresponding pid if found,
 * otherwise returns NULL pointer. Must test for NULL.
 *
 */

pnode* pnode_get_node_by_pid(int pid) {

	pnode *tmp = head;

	/* Iterate through ready queue */
	while (tmp && tmp->next && tmp->pid != pid && tmp->next != head) 
		tmp = tmp->next;
	
	/* If tmp is not node requested, iterate through blocked queue */
	if ((tmp && tmp->pid != pid && blocked) || !head) {

		tmp = blocked;

		while (tmp && tmp->next && tmp->pid != pid)
			tmp = tmp->next;

	}

	if (tmp && tmp->pid != pid) tmp = NULL;

	return tmp;

}


/*
 * pnode_destroy
 *
 * Destroy node pointed to in argument. Returns -1 if node is null.
 *
 */

int pnode_destroy(pnode *node) {

	if (!node)
		return -1;
	else {
		free(node->name);
		free(node);
		return 0;
	}

}

/*
 * pnode_add_ready
 *
 * Add node to ready queue. Takes pnode to add as argument.
 *
 */

void pnode_add_ready(pnode *proc) {

	/* Set process state */
	proc->state = READY;

	/* If list is empty */
	if (!head) {
		
		/* Adjust pointers */
		head = tail = proc;
		proc->next = proc;
		proc->prev = proc;

		/* Block idle process, start new process and reset alarm */
		kill(idle_proc->pid, SIGSTOP);
		kill(proc->pid, SIGCONT);
		alarm(3);

	/* If list is not empty */
	} else {

		/* Adjust pointers */
		tail->next = proc;

		proc->next = head;
		proc->prev = tail;

		tail = proc;
		head->prev = tail;

	}

}

/* 
 * pnode_remove_ready
 *
 * Remove node from ready queue. Takes pnode to remove as argument.
 *
 */

void pnode_remove_ready(pnode *proc) {

	/* If there is more than one node in the ready list */
	if (head != tail) {
	
		/* If head is node to remove */
		if (head == proc) { 
		
			/* Adjust head and tail pointers */
			head = head->next;
			tail->next = head;
			head->prev = tail;

			/* In case there is only 2 nodes in the list */
			if (head->next == proc) head->next = head;
		
		/* If tail is node to remove */
		} else if (tail == proc) {

			/* Adjust head and tail pointers */
			tail = tail->prev;
			head->prev = tail;
			tail->next = head;
		
			if (tail->prev == proc) tail->prev = tail;

		/* Any other nodes */
		} else
			proc->prev->next = proc->next;

	/* If there is only one node in the list */
	} else
		head = tail = NULL;


}

/*
 * pnode_add_blocked
 *
 * Add node to blocked queue. Takes pnode to add as argument.
 *
 */

void pnode_add_blocked(pnode *proc) {

	proc->state = BLOCKED;
	proc->next = NULL;
	proc->prev = NULL;

	/* If list is empty */
	if (!blocked) blocked = proc;
	
	/* Else, wedge node at the front of the list */
	else {
		proc->next = blocked;
		blocked->prev = proc;
		blocked = proc;
	}

}

/* 
 * pnode_remove_blocked
 *
 * Remove node from blocked queue. Takes pnode to remove as argument.
 *
 */

void pnode_remove_blocked(pnode *proc) {

	/* Adjust pointers */
	if (proc->prev) proc->prev->next = proc->next;
	if (proc->next) proc->next->prev = proc->prev;

	/* If node is alone in list */
	if (blocked == proc && !proc->prev && !proc->next) blocked = NULL;
	
	/* Else if node has valid previous pointer, point blocked to previous node */
	else if (blocked == proc && proc->prev) blocked = proc->prev;

	/* Else if node has valid next pointer, point blocked to next node */
	else if (blocked == proc && proc->next) blocked = proc->next;
	

}

