/*
 * @Author:	Jeff Berube
 * @Title:	proc.h
 *
 * @Description: Contains process related functions
 *
 * @Functions:
 *
 * 	add_process_ready Adds process to ready queue.
 *
 *
 * 	spawn_process	Spawns a new process in the scheduler. Adds
 * 			process to the process table.
 *
 * 	exec_process	Runs executable within sched directory.
 *
 * 	block_process	Sets process state to BLOCKED. Stops process
 * 			if running.
 *
 * 	run_process	Sets process state to READY. Puts process
 * 			at end of the ready queue.
 *
 * 	kill_process	Kills a process in the process table.
 *
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <errno.h>

#ifndef __pnode_h_
	#include "pnode.h"
#endif

extern int running_pid, pid, fd[2];

void add_process_ready(pnode *proc);

int spawn_process(char name[32]);

void exec_proces(char *filename);

void block_process(int pid);

void run_process(int pid);

void kill_process(int pid);

