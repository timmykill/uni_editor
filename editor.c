#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>

#include "term_driver.h"
#include "elements.h"
#include "utils.h"


/* global vars */
struct line * curr_l;
unsigned int gap_start, gap_end;
unsigned int curs_l;

struct block * new_blk()
{
	struct block *b;
	b = malloc(sizeof(struct block));
	b->first = NULL;
	b->last = NULL;
	b->s = 0;
	return b;
}

struct line * new_line()
{
	struct line *l;
	l = malloc(sizeof(struct line));
	l->val = NULL;
	l->s = 0;
	l->gap = false;
	l->cont = NULL;
	l->next = NULL;
	l->prev = NULL;
}

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

struct page load_page(FILE* fp)
{
	const size_t BLK_VEC_SIZE = 5;
	const size_t STRING_SIZE = 100;
	struct page pg;
	struct block * tmp_blk;
	size_t tmp_blk_size = 0;
	struct block *tmp_blk_v[BLK_VEC_SIZE];
	size_t tmp_blk_v_size = 0;
	struct block * blk_v;
	struct line * tmp_l;
	char tmp_s[STRING_SIZE];
	size_t tmp_s_size = 0;
	int tmp_c;
	
	/* init memory */
	tmp_blk = new_blk();
	tmp_l = new_line();
	tmp_blk_v[tmp_blk_v_size++] = tmp_blk;
	tmp_blk->first = tmp_l;

	do {
		/* load block */
		if (tmp_blk->s >= BLOCK_SIZE){
			if (tmp_blk_v_size +1>= BLK_VEC_SIZE) {
				/* TODO what to do here */
				die("Not implemented, BLK_SIZE_MAX");
			} else {
				tmp_blk->last = tmp_l;
				tmp_blk->s = tmp_blk_size;
				tmp_blk = new_blk();
				tmp_blk_v[tmp_blk_v_size++] = tmp_blk;
				tmp_blk_size = 0;
			}
		} else {
			/* TODO more error checking */
			tmp_c = fgetc(fp);
			if (tmp_s_size +1 >= STRING_SIZE){
				/*TODO what to do here */
				die("Not implemented, STRING_SIZE_MAX");
			}
			if(tmp_c == '\n'){
				tmp_l->next = new_line();
				/*sizeof(char) redundant, still it's correct*/
				tmp_s[tmp_s_size++] = '\0';
				tmp_l->s = tmp_s_size;
				tmp_l->val = malloc(sizeof(char) * tmp_s_size);
				memcpy(tmp_l->val, tmp_s, sizeof(char) * tmp_s_size);
				(tmp_l->next)->prev = tmp_l;
				tmp_l = tmp_l->next;
				tmp_s_size = 0;
				tmp_blk_size++;
			} else {
				tmp_s[tmp_s_size++] = tmp_c;
			}

		}
	} while (tmp_c != EOF);
	/* we still have the last line if file didnt end with \n
	tmp_l->next = new_line();
	tmp_l->val = malloc(sizeof(char) * (tmp_s_size + 1));
	memcpy(tmp_l->val, tmp_s, sizeof(char) * tmp_s_size);
	(tmp_l->next)->prev = tmp_l;
	tmp_l = tmp_l->next;
	*/
	tmp_blk->last = tmp_l;
	tmp_blk->s = tmp_blk_size;
	pg.blk_v = malloc(sizeof(struct block *) * tmp_blk_v_size);
	memcpy(pg.blk_v, tmp_blk_v, sizeof(struct block *) * tmp_blk_v_size);
	pg.s = tmp_blk_v_size;
	return pg;
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
				for(j = 0; j < gap_start; j++)
					putchar(l->val[j]);
				for(j = gap_end; l->val[j] != '\0'; j++)
					putchar(l->val[j]);
				printf("\r\n");
			} else {
				printf("%s\r\n", l->val);
			}
			l = l->next;
		}
	}
}

void save_to_file(FILE * fp, struct page pg)
{
	struct line *l;
	int i;
	rem_gap();
	rewind(fp);
	for (i = 0; i < pg.s; i++) 
		for(l = pg.blk_v[i]->first; l != NULL; l = l->next)
			fprintf(fp, "%s\n", l->val);
	make_gap();
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

int main(int argc, char *argv[])
{
	struct winsize w;
	int tmp;
	struct page pg;
	bool ref; //refresh screen
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
		} else if ((tmp == 127 || tmp == 8 ) && gap_start > 0){
			gap_start--;
		} else if (tmp == 's'){
			save_to_file(out, pg);
		} else {
			curr_l->val[gap_start++] = (char) tmp;
		}	
		print_page(pg);
		print_cursor(gap_start, curs_l);
	} while (tmp != 'q');
}
