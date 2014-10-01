/*
 * @Author:	Jeff Berube
 * @Title:	proc.h
 *
 * @Description: Contains process related functions
 *
 * @Functions:
 *
 * 	spawn_process	Spawns a new process in the scheduler. Adds
 * 			process to the process table.
 *
 * 	kill_process	Kills a process in the process table.
 */

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <errno.h>

#ifndef __pnode_h_
	#include "pnode.h"
#endif

extern pnode *head, *tail, *idle_proc;
extern int running_pid, pid, fd[2];

int spawn_process(char name[32]);

void kill_process(int pid);

