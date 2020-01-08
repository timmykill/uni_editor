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

struct block * new_blk()
{
	struct block *b;
	b = malloc(sizeof(struct block));
	b->first = NULL;
	b->last = NULL;
	b->lenght = 0;
	return b;
}

struct line * new_line()
{
	struct line *l;
	l = malloc(sizeof(struct line));
	l->val = NULL;
	l->s = 0;
	l->cont = NULL;
	l->next = NULL;
	l->prev = NULL;
}

struct page load_page(FILE* fp)
{
	const size_t BLK_VEC_SIZE = 5;
	const size_t STRING_SIZE = 100;
	struct page pg;
	struct block * tmp_blk;
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
		if (tmp_blk->lenght >= BLOCK_SIZE){
			if (tmp_blk_v_size +1>= BLK_VEC_SIZE) {
				/* TODO what to do here */
				die("Not implemented, BLK_SIZE_MAX");
			} else {
				tmp_blk->last = tmp_l;
				tmp_blk = new_blk();
				tmp_blk_v[tmp_blk_v_size++] = tmp_blk;
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

	pg.blk_v = malloc(sizeof(struct block *) * tmp_blk_v_size);
	memcpy(pg.blk_v, tmp_blk_v, sizeof(struct block *) * tmp_blk_v_size);
	pg.s = tmp_blk_v_size;
	return pg;
}

void print_page(struct page pg)
{
	size_t i;
	struct line * l;
	for(i = 0; i < pg.s; i++){
		l = pg.blk_v[i]->first;
		while(l != NULL){
			printf("%s\r\n", l->val);
			l = l->next;
		}
	}
}


int main(int argc, char *argv[])
{
	struct winsize w;
	int tmp;
	struct page pg;
	struct text_box_start ts;
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

	/* Initialize text box and cursor*/
	ts.block = 0;
	ts.line = 0;
	
	/* Load page */
	pg = load_page(fp); 

	/* Print content of block */
	print_page(pg);

	/* 	Get user input	*/
	do {
		tmp = getchar();
		if (tmp == 'k'){
		} else if (tmp == 'j'){
		} else if (tmp == 'l'){
		} else if (tmp == 'h'){
		}
	} while (tmp != 'q');
}
