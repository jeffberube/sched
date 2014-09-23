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

	return node;

}

/*
 * pnode_destroy_by_name
 *
 * Find and destroy a node using its name. Returns -1 if node couldn't be found.
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
 * pnode_get_node_by_name
 *
 * Iterates through processes and returns process with corresponding name if found,
 * otherwise returns NULL pointer. Must test for NULL.
 *
 */

pnode* pnode_get_node_by_name(char name[32]) {
	
	pnode *tmp = head;

	while(tmp->next && tmp->next->name != name) tmp = tmp->next;	
	
	return tmp;
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

	while(head->next && head->next->pid != pid) tmp = head->next;

	return tmp;

}


