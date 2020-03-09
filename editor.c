#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <stdint.h>

#include "term_driver.h"
#include "types.h"
#include "utils.h"
#include "file.h"
#include "second_buffer.h"

#define PRINT_CURSOR() print_cursor(gap_start - start_long_line, curs_l)


/* global vars */
struct line * curr_l;
uint_fast16_t 	gap_start, gap_end, held_gap_start, 
				start_long_line, 
				curs_l, 
				cols, rows;


/**
	Undoes whatever make_gap does
*/
void rem_gap()
{
	size_t n_size, g_size;
	char * normal_val;
	g_size = curr_l->s;
	n_size = g_size - (gap_end - gap_start);
	
	normal_val = malloc(n_size);
	memcpy(normal_val, curr_l->val, gap_start);
	memcpy(normal_val + gap_start, curr_l->val + gap_end, g_size - gap_end);

	curr_l->s = n_size;
	free(curr_l->val);
	curr_l->val = normal_val;
}

/**
	Creates the gap buffer for the current line
*/
void make_gap()
{
	const uint_fast8_t GAP_SIZE = 10; 
	size_t n_size, g_size; //these are for convinience
	char * gapped_val;

	n_size = curr_l->s;
	g_size = n_size + GAP_SIZE;

	/* 
		Creates gapped char vector
		copies first part (before the cursor) at the start of the buffer
		copies last part (after the cursor) at the end of the buffer
	*/
	gapped_val = (char*) malloc(g_size);
	memcpy(gapped_val, curr_l->val, gap_start);
	memcpy(gapped_val + gap_start + GAP_SIZE , curr_l->val + gap_start, n_size - gap_start);

	/* sets gap_start (cursor position) and gap_end (first char after the gap) pointers */
	gap_end = gap_start + GAP_SIZE;
	curr_l->s = g_size;
	free(curr_l->val);
	curr_l->val = gapped_val;
}

void print_page(struct page pg)
{
	size_t i;
	struct line * l;
	for(i = 0; i < pg.s; i++){
		l = pg.blk_v[i]->first;
		while(l != NULL){
			if (l == curr_l){
				if ((size_t) cols <  l->s) {
					if (gap_start < start_long_line)
						die("something really wrong");
					write(STDOUT_FILENO, l->val + start_long_line, gap_start - start_long_line);
					write(STDOUT_FILENO, l->val + gap_end, cols + start_long_line - gap_start);
				} else {
					write(STDOUT_FILENO, l->val, gap_start);
					write(STDOUT_FILENO, l->val + gap_end, l->s - gap_end);	
				}
			} else {
				if ((size_t) cols < l->s){
					write(STDOUT_FILENO, l->val, cols - 1);
					write(STDOUT_FILENO, "\x1B[7m>\x1b[0m", 10);
				} else {
					write(STDOUT_FILENO, l->val, l->s);
				}
			}
			write(STDOUT_FILENO, "\r\n", 3);
			l = l->next;
		}
	}
}

/* footer a la nano */
void print_footer(char *msg)
{
	uint_fast16_t i;
	char *c;
	//delete last 2 lines
	clear_line(rows-2);
	for (i = 0; i < cols; i++)
		write(STDOUT_FILENO, "#", 2);
	write(STDOUT_FILENO, "\r\n", 3);
	clear_line(rows-1);
	write(STDOUT_FILENO, "         ", 10);
	write(STDOUT_FILENO, "s: save in test.txt file, q: quit, arrows to move", 50);
	write(STDOUT_FILENO, "         ", 10);
	for (c = msg; *c != '\0'; c++) {
		write(STDOUT_FILENO, c, 1);
	}
	write(STDOUT_FILENO, "\r\n", 3);
}


void set_new_gap_start()
{
	if (gap_start < curr_l->s){
		if (held_gap_start > gap_start && held_gap_start < curr_l->s)
			gap_start = held_gap_start;
		else if (held_gap_start >= curr_l->s)
			gap_start = curr_l->s -1;
	} else {
		held_gap_start = gap_start;
		gap_start = curr_l->s -1;
	}	
}


void capture_arrow(unsigned int y_const)
{
	/* for now every line starts at zero */
	getchar();
	switch(getchar()) {
		case 'A':
			if (curs_l == 0)
				break;
			/* else, go up */
			rem_gap();
			if (curr_l->prev == NULL)
				die("Line has no prev");
			curr_l = curr_l->prev;
			curs_l--;
			set_new_gap_start();
			start_long_line = 0;
			make_gap();
			break;
		case 'B':
			if (curs_l >= y_const)
				break;
			/* else, go down */
			if (curr_l->next != NULL){
				rem_gap();
				curs_l++;
				curr_l = curr_l->next;
				set_new_gap_start();
				start_long_line = 0;
				make_gap();
			}
			break;
		case 'C': /* freccia a destra */
			if (gap_end < curr_l->s-1 && gap_start < cols + start_long_line){
				curr_l->val[gap_start++] = curr_l->val[gap_end++];
				if (gap_start - start_long_line == cols -1)
					start_long_line += 10;
			}
			break;
		case 'D': /* freccia a sinistra */
			if (gap_start != 0) {
				curr_l->val[--gap_end] = curr_l->val[--gap_start];
				if (start_long_line != 0 && gap_start == start_long_line)
					start_long_line -= 10;
			}
			break;
	}
}


/**
	Create a new line, append content of previous line before the insertion
	Sets the linked list
	
	Right now it works with gapped lines, but i would like to work with normal lines, and rem/make the gap outside of this function
*/
void newline()
{
	/* Sets memory */
	struct line *l;
	size_t nl_s;
	l = new_line();
	
	/* copies the portion of the previous line after the cursor */
	nl_s = curr_l->s - gap_end; 
	l->val = malloc(nl_s);
	memcpy(l->val, curr_l->val + gap_end, nl_s);
	l->s = nl_s;
	gap_end = curr_l->s - 1; 
	
	/* sets linked list */
	l->next = curr_l->next;
	curr_l->next = l;
	l->prev = curr_l;
	curs_l++;
	
	/* i dont like having rem_gap and make_gap here*/
	rem_gap();
	curr_l = l;
	start_long_line = 0;
	held_gap_start = 0;
	gap_start = 0;
	make_gap();
}

void remline()
{
	struct line *tmp_l;
	char * new_v;
	curr_l->next->prev = curr_l->prev;
	curr_l->prev->next = curr_l->next;
	tmp_l = curr_l;
	curr_l = curr_l->prev;
	/* append deleted line to current */
	new_v = (char*) realloc(curr_l->val, tmp_l->s + curr_l->s);
	memcpy(new_v + curr_l->s, tmp_l->val, tmp_l->s);
	curr_l->s += tmp_l->s;
	curr_l->val = new_v;
	free(tmp_l->val);
	free(tmp_l);
	make_gap();
	curs_l--;
}

/**
	Keeps user input focused on the footer (second buffer)
	gets file name
	opens the file and serves it to save_to_file
*/
void get_file_and_save(struct page pg)
{	
	char tmp;
	struct winsize w;
	FILE* fp;
	print_footer(buf2_get_content());
	do {
		if (read(STDIN_FILENO,&tmp,1)){
			/* delete or backspace */
			if (tmp == 127 || tmp == 8)
				buf2_rm_char();
			else if (tmp != '\n')
				buf2_put_char(tmp);
			print_footer(buf2_get_content());
		} else {
			/*update the size of the terminal*/
			ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
			cols = w.ws_col;
			rows = w.ws_row;
		}
	} while (tmp != '\n');
	fp = fopen(buf2_get_content(), "w");
	save_to_file(fp, pg);
}


int main(int argc, char *argv[])
{
	struct winsize w;
	uint_fast16_t tmp;
	struct page pg;
	FILE *in;

	/* If no file specified, exit */
	if (argc != 2)
		die("Wrong number of args, specify a file");

	/* Load file */
	in = fopen(argv[1], "r");
	if (in == NULL)
		die("Couldn't read the file");
	
	/* sets standard write path (conviniance) */
	buf2_put_string("test.txt");
	
	/* Clear screen, set it raw, and get terminal size */
	clear_screen();
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	cols = w.ws_col;
	rows = w.ws_row;
	prep_term();
	atexit(restore_term);

	/* Load page */
	pg = load_page(in); 
	curr_l = pg.blk_v[0]->first;
	gap_start = 0;
	held_gap_start = 0;
	start_long_line = 0;
	curs_l = 0;
	make_gap();

	/* if file is bigger than screen die, for now */
	if( pg.s > 1 || pg.blk_v[0]->s > (size_t) w.ws_row)
		die("File is too big, no scrolling for now");

	/* Print content of page*/
	clear_screen();
	print_page(pg);
	print_footer("");

	/* Initialize text box and cursor*/
	PRINT_CURSOR();

	/* 	Get user input	*/
	do {
		char *msg = "";
		if(read(STDIN_FILENO,&(tmp),1)){
			if (tmp == '\033'){
				capture_arrow(pg.blk_v[0]->s);
		/*backspace or delete chars*/
			} else if (tmp == 127 || tmp == 8){
				if (gap_start > 0){
					gap_start--;
				} else if (curs_l > 0){
					remline();
					(pg.blk_v[0]->s)--;
				}
			} else if (tmp == '\n'){
				newline();
				(pg.blk_v[0]->s)++;
			} else if (tmp == 's'){
				rem_gap();
				get_file_and_save(pg);
				make_gap();
				msg = "Saved file";
			} else {
				if(gap_end - gap_start == 1){
					rem_gap(); //this could be useless
					make_gap();
				}
				if (gap_start < cols)
					curr_l->val[gap_start++] = (char) tmp;
			}
		} else {
			/*update the size of the terminal*/
			ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
			cols = w.ws_col;
			rows = w.ws_row;
		}	
		clear_screen();
		print_page(pg);
		print_footer(msg);
		PRINT_CURSOR();
	} while (tmp != 'q');
}
