#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>

#include "term_driver.h"
#include "types.h"
#include "utils.h"
#include "file.h"

/* global vars */
struct line * curr_l;
unsigned int gap_start, gap_end;
unsigned int curs_l;

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

void make_gap()
{
	unsigned int GAP_SIZE = 100;
	size_t n_size, g_size;
	char * gapped_val;

	n_size = curr_l->s;
	g_size = n_size + GAP_SIZE;

	gapped_val = malloc(g_size);
	memcpy(gapped_val, curr_l->val, gap_start);
	memcpy(gapped_val + gap_start + GAP_SIZE , curr_l->val + gap_start, n_size - gap_start);

	gap_end = gap_start + GAP_SIZE;
	curr_l->s = g_size;
	free(curr_l->val);
	curr_l->val = gapped_val;
}


void print_page(struct page pg)
{
	size_t i, j;
	struct line * l;
	clear_screen();
	for(i = 0; i < pg.s; i++){
		l = pg.blk_v[i]->first;
		while(l != NULL){
			if (l == curr_l){
				write(STDOUT_FILENO, l->val, gap_start);
				write(STDOUT_FILENO, l->val + gap_end, l->s - gap_end);
			} else {
				write(STDOUT_FILENO, l->val, l->s);
			}
				write(STDOUT_FILENO, "\r\n", 2);
			l = l->next;
		}
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
			gap_start = 0;
			if (curr_l->prev == NULL)
				die("Line has no prev");
			curr_l = curr_l->prev;
			curs_l--;
			make_gap();
			break;
		case 'B':
			if (curs_l >= y_const)
				break;
			/* else, go down */
			rem_gap();
			gap_start = 0;
			if (curr_l->next != NULL){
				curs_l++;
				curr_l = curr_l->next;
				make_gap();
			} else {
				make_gap();
			}
			break;
		case 'C':
			if (gap_end >= curr_l->s)
				break;
			curr_l->val[gap_start] = curr_l->val[gap_end];
			gap_start++;
			gap_end++;
			break;
		case 'D':
			if (gap_start == 0)
				break;
			curr_l->val[gap_start] = curr_l->val[gap_end];
			gap_start--;
			gap_end--;
			break;
	}
}

void newline()
{
	struct line *l;
	size_t nl_s;
	l = new_line();
	
	nl_s = curr_l->s - gap_end; 
	l->val = malloc(nl_s);
	memcpy(l->val, curr_l->val + gap_end, nl_s);
	l->s = nl_s;
	gap_end = curr_l->s;
	
	l->next = curr_l->next;
	curr_l->next = l;
	l->prev = curr_l;
	curs_l++;
	
	rem_gap();
	curr_l = l;
	gap_start = 0;
	make_gap();
}

int main(int argc, char *argv[])
{
	struct winsize w;
	struct line *tmp_l;
	int tmp;
	struct page pg;
	FILE *in, *out;

	/* If no file specified, exit */
	if (argc != 2)
		die("Wrong number of args, specify a file");

	/* Load file */
	in = fopen(argv[1], "r");
	out = fopen("test.txt", "w");
	if (in == NULL || out == NULL)
		die("Couldn't read|write the file");
	
	/* Clear screen, set it raw, and get terminal size */
	clear_screen();
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	prep_term();
	atexit(restore_term);

	/* Load page */
	pg = load_page(in); 
	curr_l = pg.blk_v[0]->first;
	gap_start = 0;
	curs_l = 0;
	make_gap();

	/* if file is bigger than screen die, for now */
	if( pg.s > 1 || (unsigned short) pg.blk_v[0]->s > w.ws_row)
		die("File is too big, no scrolling for now");

	/* Print content of page*/
	print_page(pg);

	/* Initialize text box and cursor*/
	print_cursor(gap_start, curs_l);

	/* 	Get user input	*/
	do {
		tmp = getchar();
		if (tmp == '\033'){
			capture_arrow(pg.blk_v[0]->s);
		/*backspace or delete chars*/
		} else if (tmp == 127 || tmp == 8){
			if (gap_start > 0){
				gap_start--;
			} else if (curs_l > 0){
				curr_l->next->prev = curr_l->prev;
				curr_l->prev->next = curr_l->next;
				tmp_l = curr_l;
				curr_l = curr_l->prev;
				free(tmp_l->val);
				free(tmp_l);
				make_gap();
				curs_l--;
				(pg.blk_v[0]->s)--;
			}
		} else if (tmp == '\n'){
			newline();
			(pg.blk_v[0]->s)++;
		} else if (tmp == 's'){
			rem_gap();
			save_to_file(out, pg);
			make_gap();
		} else {
			curr_l->val[gap_start++] = (char) tmp;

		}	
		print_page(pg);
		print_cursor(gap_start, curs_l);
	} while (tmp != 'q');
}
