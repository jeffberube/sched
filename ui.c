/*
 * @Author:	Jeff Berube
 * @Title:	UI
 *
 * @Description: UI rendering functions for scheduler
 *
 */

#include <stdio.h>
#include <ncurses.h>

#include "ui.h"

/*
 * print_ui()
 *
 * Prints all ui elements like labels and lines
 *
 */

void print_ui() {

	/* Print label "Output:" */
	mvprintw(VPADDING, HPADDING, "Output: \n");	

	/* Print label "Process Table:" */
	mvprintw(VPADDING, ncols * 0.6 - HPADDING, "Process Table (PID, Name): \n");

	/* Define process table x coordinate */
	int pt_x = ncols * 0.6 - (2 * HPADDING);

	/* Print line under Output and Process Table labels */
	attron(COLOR_PAIR(4));

	int i = 0;

	while (i < ncols - (2 * HPADDING)) {

		mvprintw(VPADDING + 1, HPADDING + i, 
				(i > pt_x - 10 && i < pt_x) ? " " : "\u2500");
		
		i++;

	}

	attroff(COLOR_PAIR(4));

	/* Print line over command line */
	attron(COLOR_PAIR(4));

	i = 0;

	while (i < ncols - (2 * HPADDING)) {

		mvprintw(nrows - FOOTER , HPADDING + i, "\u2500");
		i++;
	}

	attroff(COLOR_PAIR(4));

	/* Print command line label */
	mvprintw(nrows - FOOTER + 1, HPADDING, "COMMAND > %s", comm);


}

void print_log() {

	/* Define starting coordinates */
	int y = VPADDING + 2;
	int x = HPADDING + 1;

	/* Print log under Piped Output */
	int i = 0;
	int logrows = nrows - HEADER - FOOTER;
	int startline = logcount < logrows ? 0 : lastline % logrows;

	
	while (i < logrows) {
		
		int line = (startline + i) % logrows;
	
		if(logtable[line]) 
			mvprintw(y + i, x, "%s", logtable[line]);

		i++;

	}

}

/*
 * ani_char()
 *
 * Returns index of spinning character
 *
 */

int ani_char() {

	static int i = 0, ch = 0;

	i++;
	i = i % 500;

	if (!i) {
		ch++;
		ch = ch % 4;
	}

	return ch;

}

/*
 * print_proc_table()
 *
 * Prints process table to screen
 *
 */

void print_proc_table() {

	/* Define starting coordinates */
	int y = VPADDING + 2;
	int x = ncols * 0.6 - HPADDING;

	/* Print animated character beside running process */
	char ani[] = {'/', '-', '\\', '|', 0};
	mvprintw(y, x - 2, "%c", ani[ani_char()]);
	
	/* Maintains which row to print to */
	int i = 0;

	/* If there is a process in the list */
	if (head) {
		
		mvprintw(y + i, x, "%d\t%s", head->pid, head->name);	

		i++;

		/* If there is more than one process in the list */
		if (head->next && head != tail) {
		
			pnode *tmp = head;

			while (tmp->next && tmp->next != head) {
			
				tmp = tmp->next;

				mvprintw(y + i, x, "%d\t%s", tmp->pid, tmp->name);

				i++;
			}
		
		}

	}

	/* Print idle process */
	attron(COLOR_PAIR(3));
	mvprintw(y + i, x, "%d\tIdle\n", idle_proc->pid);
	attroff(COLOR_PAIR(3));

}

/*
 * update_screen
 *
 * Updates the current screen.
 *
 */

void update_screen() {

	//unsigned char line_output[(int)(ncols * 0.6 - (2 * PADDING) + 1)];

	erase();	
	
	print_log();

	print_proc_table();
	
	print_ui();
	
	refresh();

}

/*
 * init_ncurses()
 *
 * Initiate ncurses context and set parameters
 *
 */

void init_ncurses() {

	/* Setup locale to use universal characters */
	setlocale(LC_CTYPE, "");

	/* Init ncurses and set environment */
	initscr();
	noecho();
	halfdelay(1);
	keypad(stdscr, true);
	getmaxyx(stdscr, nrows, ncols);

	/* Setup ncurses colors */
	start_color();

	/* If terminal can change colors, change cyan to light gray */
	if (can_change_color())
		init_color(COLOR_CYAN, 600, 600, 600);

	/* Initiate different color pairs */
	init_pair(1, COLOR_WHITE, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	init_pair(3, COLOR_CYAN, COLOR_BLACK);
	init_pair(4, COLOR_BLUE, COLOR_BLACK);

}

/*
 * end_ncurses();
 *
 * Destroy ncurses context when program ends
 *
 */

void end_ncurses() {

	endwin();
	
}
