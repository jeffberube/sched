/*
 * @Author:	Jeff Berube
 * @Title:	UI
 *
 * @Description: UI rendering functions for scheduler
 *
 * @Constants:
 *
 * 	PADDING		Padding value to use for layout
 * 	HEADER		Space occupied by top labels and separating lines
 * 	FOOTER		Space occupied by command line and separating line
 *
 * @Functions:
 *
 * 	print_ui()	Prints all ui elements like labels and lines
 * 	
 * 	print_log()	Iterates trough logged piped output and prints to screen
 * 	
 * 	print_proc_table()	Prints process table
 *
 * 	ani_char()	Returns index of animated character in process table.
 *
 * 	update_screen()	Updates screen
 *
 * 	init_ncurses()	Initiates ncurses context. Called once.
 *
 * 	end_ncurses()	Ends ncurses context when exiting program.
 *
 */

#include <stdio.h>
#include <ncurses.h>
#include <locale.h>
#include <ctype.h>
#include <string.h>

#ifndef __pnode_h_
	#include "pnode.h"
#endif

#define VPADDING 	1
#define HPADDING 	2
#define	HEADER		3
#define FOOTER		3

extern pnode *head, *tail, *idle_proc;

extern int ncols, nrows, comm_ptr, lastline, logcount;
extern char *logtable[128];
extern char comm[64];
extern char *errstr;

void print_ui();

void print_log();

void print_proc_table();

int ani_char(); 

void update_screen();

void init_ncurses();

void end_ncurses();



