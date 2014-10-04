/*
 * @Author:	Jeff Berube
 * @Title:	pnode (Doubly linked node for process scheduler)
 *
 */


#define __pnode_h_

typedef enum pstate {READY, RUNNING, BLOCKED} pstate;

typedef struct pnode pnode;

struct pnode {
	pnode	*next;
	pnode	*prev;
	int	pid;
	char	*name;
	pstate	state;
};

extern pnode *head, *tail, *idle_proc;
extern char errstr[64];

pnode* pnode_create(int pid, char *name);

void pnode_destroy_by_name(char name[32]);

pnode* pnode_get_node_by_pid(int pid); 


