/*
 * @Author:	Jeff Berube
 * @Title:	pnode (Doubly linked node for process scheduler)
 *
 * @Description: This library contains process nodes and their functions.
 *
 * 	pnode_create		Creates a new node
 *
 * 	pnode_destroy		Frees the memory associated to an
 * 				existing node
 *
 * 	pnode_get_node_by_pid	Returns a pointer to the node associated
 * 				with a pid or null if node not found.
 *
 * 	pnode_add_ready		Adds a node to the ready queue.
 *
 * 	pnode_remove_ready	Removes a node from the ready queue.
 *
 * 	pnode_add_blocked	Adds a node to the blocked queue.
 *
 * 	pnode_remove_blocked	Removes a node from the blocked queue.
 *
 */


#define __pnode_h_

#include <stdlib.h>
#include <signal.h>

typedef enum pstate {READY, RUNNING, BLOCKED} pstate;

typedef struct pnode pnode;

struct pnode {
	pnode	*next;
	pnode	*prev;
	int	pid;
	char	*name;
	pstate	state;
};

extern pnode *head, *tail, *blocked, *idle_proc;
extern char errstr[64];

pnode* pnode_create(int pid, char *name);

pnode* pnode_get_node_by_pid(int pid); 

int pnode_destroy(pnode *node);

void pnode_add_ready(pnode *node);

void pnode_remove_ready(pnode *node);

void pnode_add_blocked(pnode *node);

void pnode_remove_blocked(pnode *node);

