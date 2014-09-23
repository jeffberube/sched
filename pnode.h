/*
 * @Author:	Jeff Berube
 * @Title:	pnode (Doubly linked node for process scheduler)
 *
 */


/*
 * pnode
 *
 * Doubly linked node used to keep track of scheduled processes.
 *
 */

typedef struct pnode pnode;

struct pnode {
	pnode	*next;
	pnode	*prev;
	int	pid;
	char	*name;
};

extern pnode* head;

pnode* pnode_create(int pid, char *name);

void pnode_destroy_by_name(char name[32]);

pnode* pnode_get_node_by_name(char name[32]);

pnode* pnode_get_node_by_pid(int pid); 


