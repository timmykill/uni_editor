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
struct cursor curs;
struct line * curr_l;
unsigned int gap_start, gap_end;

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
	/* TODO later */
}

void make_gap()
{
	unsigned int GAP_SIZE = 100;
	size_t n_size, g_size;
	char * gapped_val;
	
	if (curr_l->gap == true)
		return;

	n_size = curr_l->s;
	g_size = n_size + GAP_SIZE;

	gapped_val = malloc(sizeof(char) * g_size);
	memcpy(gapped_val, curr_l->val, n_size/2);
	memcpy(gapped_val + n_size/2 + GAP_SIZE, curr_l->val + n_size/2, n_size/2);

	curr_l->gap_start = n_size/2; 
	curr_l->gap_end = n_size/2 + GAP_SIZE ;

	curr_l->gap = true;
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
	/* j to catch cursor */
	size_t i, j;
	struct line * l;
	clear_screen();
	for(i = 0; i < pg.s; i++){
		l = pg.blk_v[i]->first;
		while(l != NULL){
			if (l->gap == true){
				for(j = 0; j < l->gap_start; j++)
					putchar(l->val[j]);
				for(j = l->gap_end; l->val[j] != '\0'; j++)
					putchar(l->val[j]);
				printf("\r\n");
			}
			printf("%s\r\n", l->val);
			l = l->next;
		}
	}
}

void manage_cursor(unsigned int y_const)
{
	/* for now every line starts at zero */
	getchar();
	switch(getchar()) {
		case 'A':
			if (curs.y > 1){
				curs.y--;
				curr_l = curr_l->prev;
				make_gap();
			}
			curs.x = 0;
			break;
		case 'B':
			if (curs.y < y_const) {
				curs.y++;
				curr_l = curr_l->next;
				make_gap();
			}
			curs.x = 0;
			break;
		case 'C':
			if (curs.x < curr_l->s)
				curs.x++;
			break;
		case 'D':
			if (curs.x > 0)
				curs.x--;
			break;
	}
}

int main(int argc, char *argv[])
{
	struct winsize w;
	int tmp;
	struct page pg;
	bool ref; //refresh screen
	FILE *fp;

	/* If no file specified, exit */
	if (argc != 2)
		die("Wrong number of args, specify a file");

	/* Load file */
	fp = fopen(argv[1], "r");
	if (fp == NULL)
		die("Couldn't read the file");
	
	/* Clear screen, set it raw, and get terminal size */
	clear_screen();
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	prep_term();
	atexit(restore_term);

	/* Load page */
	pg = load_page(fp); 
	curr_l = pg.blk_v[0]->first;

	/* if file is bigger than screen die, for now */
	if( pg.s > 1 || (unsigned short) pg.blk_v[0]->s > w.ws_row)
		die("File is too big, no scrolling for now");

	/* Print content of page*/
	print_page(pg);
	make_gap();

	/* Initialize text box and cursor*/
	curs.x = 1;
	curs.y = 1;
	print_cursor(1,1);


	/* 	Get user input	*/
	do {
		tmp = getchar();
		if (tmp == '\033')
			manage_cursor(pg.blk_v[0]->s);
		print_page(pg);
		print_cursor(curs.x, curs.y);
	} while (tmp != 'q');
}
