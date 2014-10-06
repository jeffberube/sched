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
 *	show_help	Shows help window
 *
 *	history_add	Adds a command into the history
 *
 *	history_get_prev	Gets previous command in history
 *
 *	history_get_next	Gets next command in history
 *
 * 	print_ui	Prints all ui elements like labels and lines
 * 	
 * 	print_log	Iterates trough logged piped output and prints to screen
 * 	
 * 	print_proc_table	Prints process table
 *
 * 	ani_char	Returns index of animated character in process table.
 *
 * 	update_screen	Updates screen
 *
 * 	init_ncurses	Initiates ncurses context. Called once.
 *
 * 	end_ncurses	Ends ncurses context when exiting program.
 *
 */

#define _XOPEN_SOURCE_EXTENDED_

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

#define HIST_MAX	10

extern pnode *head, *tail, *idle_proc;

extern int ncols, nrows, hist_ptr, hist_count, comm_ptr, lastline, logcount;
extern char *history[HIST_MAX];
extern char *logtable[128];
extern char comm[64];
extern char errstr[128];

void print_ui();

void print_log();

void print_proc_table();

int ani_char(); 

void update_screen();

void init_ncurses();

void end_ncurses();



