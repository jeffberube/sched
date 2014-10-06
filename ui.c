/*
 * @Author:	Jeff Berube
 * @Title:	UI
 *
 * @Description: UI rendering functions for scheduler
 *
 */

#include "ui.h"

/*
 * show_help()
 *
 * Shows help window
 *
 */

void show_help() {

	

}

/*
 * log_add_line
 *
 * Adds a line to the log table
 *
 */

void log_add_line(char *buffer) {

	/* Create a string and copy buffer into it */
	char* message = malloc(strlen(buffer) + 1);

	memset(message, 0, strlen(buffer) + 1);
	strcpy(message, buffer);
	
	/* If logtable is full, free previous string and assign new one */
	if (logcount == (nrows - HEADER - FOOTER) - 1) free(logtable[lastline]);
	logtable[lastline] = message;

	lastline++;

	lastline = lastline % (nrows - HEADER - FOOTER);

	if (logcount <= (nrows - HEADER - FOOTER) - 1) logcount++;

}


/*
 * history_add
 *
 * Adds a command to the history
 *
 */

void history_add(char *buffer) {

	/* Create a string to hold command and copy buffer into it */
	char *last_comm = malloc(strlen(buffer) + 1);

	memset(last_comm, 0, strlen(buffer) + 1);
	strcpy(last_comm, buffer);

	/* If history table is full, free first element and shift array up 1 */
	if (hist_count == HIST_MAX) {
		
		free(history[0]);
		
		int i = 0;

		while (i < hist_count)
			history[i] = history[i + 1];

		history[hist_count] = last_comm;

	/* If history table isn't full, just assign and increase pointer */
	} else 
		history[hist_count++] = last_comm;

	hist_ptr = hist_count;
}

/*
 * history_get_prev
 *
 * Sets command line to one previous from last request
 *
 */

void history_get_prev() {

	memset(comm, 0, sizeof(comm));
	strcpy(comm, !hist_ptr ? history[0] : history[--hist_ptr]);
	
}

/*
 * history_get_next
 *
 * Sets command line to one more recent than last request
 *
 */

void history_get_next() {

	memset(comm, 0, sizeof(comm));
	strcpy(comm, hist_count == hist_ptr + 1 ? "" : history[++hist_ptr]);

}


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
	mvprintw(VPADDING, ncols * 0.6 - HPADDING, "Process Table (PID, Name, State):");

	/* Define process table x coordinate */
	int pt_x = ncols * 0.6 - (2 * HPADDING);

	/* Print line under Output and Process Table labels */
	attron(COLOR_PAIR(4));

	int i = 0;
	char hline[2] = {175,0};	

	move(VPADDING + 1, HPADDING);

	while (i < ncols - (2 * HPADDING)) {

		addch((i > pt_x - 10 && i < pt_x) ? ' ' : ACS_HLINE);
		
		i++;

	}

	i = 0;
	
	move(nrows - FOOTER, HPADDING);

	while (i < ncols - (2 * HPADDING)) {

		addch(ACS_HLINE);
		i++;
	}

	attroff(COLOR_PAIR(4));

	/* Print error message */
	if (errstr) {

		attron(COLOR_PAIR(2));
		
		mvprintw(nrows - FOOTER + 2, HPADDING, "%s", errstr);

		attroff(COLOR_PAIR(2));
	
	}

	/* Print command line label */
	mvprintw(nrows - FOOTER + 1, HPADDING, "[sched]$ %s", comm);


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
	i = i % 2;

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

	/* Print ready queue */
	if (head) {
		
		/* Print PID, name and state */
		mvprintw(y + i, x, "%d\t%s", head->pid, head->name);	
		mvprintw(y + i, ncols - HPADDING - 7, "RUNNING");

		i++;

		/* If there is more than one process in ready queue */
		if (head->next && head != tail) {
		
			pnode *tmp = head;

			while (tmp->next && tmp->next != head) {
			
				tmp = tmp->next;

				/* Print PID, name and state */
				mvprintw(y + i, x, "%d\t%s", tmp->pid, tmp->name);
				mvprintw(y + i, ncols - HPADDING - 5, "READY");

				i++;
			}
		
		}

	}

	/* Print idle process */
	attron(COLOR_PAIR(3));
	
	mvprintw(y + i, x, "%d\tIdle\n", idle_proc->pid);
	
	attroff(COLOR_PAIR(3));

	/* Print idle state */
	if (!head) mvprintw(y + i, ncols - HPADDING - 7, "RUNNING");
	else mvprintw(y + i, ncols - HPADDING - 5, "READY");

	i++;

	/* Print blocked queue */
	attron(COLOR_PAIR(5));

	if (blocked) {
	
		/* Print PID, name and state */
		mvprintw(y + i, x, "%d\t%s", blocked->pid, blocked->name);
		mvprintw(y + i, ncols - HPADDING - 7, "BLOCKED");

		i++;

		/* If there is more than one blocked process */
		if (blocked->next) {

			pnode *tmp = blocked;

			while (tmp->next) {
			
				tmp = tmp->next;

				mvprintw(y + i, x, "%d\t%s", tmp->pid, tmp->name);
				mvprintw(y + i, ncols - HPADDING - 7, "BLOCKED");
				i++;

			}

		}

	}	

	attroff(COLOR_PAIR(5));

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
	init_pair(5, COLOR_YELLOW, COLOR_BLACK);

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
