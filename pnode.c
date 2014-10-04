/*
 * @Author:	Jeff Berube
 * @Title:	pnode (Doubly linked node for process scheduler)
 *
 */

#include "stdlib.h"
#include "pnode.h"

/*
 * pnode_create
 *
 * Creates a new pnode and returns a pointer to the newly created pnode;
 *
 */

pnode* pnode_create(int pid, char* name) {
	
	pnode *node = malloc(sizeof(pnode));
	node->pid = pid;
	node->name = name;
	node->state = READY;

	return node;

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
		free(node);
		return 0;
	}

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

	while (tmp->next && tmp->pid != pid && tmp->next != head) 
		tmp = tmp->next;

	if (tmp->pid != pid) tmp = NULL;

	return tmp;

}


