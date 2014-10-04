/*
 * @Author:	Jeff Berube
 * @Title:	comm.h
 *
 * @Description: Contains all the command processing functions
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern char errstr[64];
extern char comm[64];
extern char args[2][64];
extern int comm_ptr;

#define SPAWN	1
#define EXEC	2
#define	BLOCK	3
#define RUN	4
#define KILL	5
#define HELP	6
#define QUIT	7

int validate_command();

int validate_param();

void parse_command();

void exec_command();
