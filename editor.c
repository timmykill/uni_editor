#include <stdlib.h>
#include <strings.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>

#include "term_driver.h"
#include "elements.h"
#include "cursor.h"

void print_cursor(struct cursor curs, struct text_box_start ts, struct block *blk){
	int x, y, i;
	struct line *curr_line;
	curr_line = blk->first;
	// block stuff
	/* get line lenght */
	for (i = 0; i <= curs.line && curr_line != NULL; i++){
		curr_line = curr_line->next;
	}
	for (i = 0; i <= curs.line_block && curr_line != NULL; i++){
		curr_line = curr_line->cont;
	}
	y = curs.line - ts.line;
	x = i * LINE_LENGHT + curs.pos;
}

struct block * load_block(FILE* fp){
	struct block *bl;
	struct line* curr_line;
	struct line* cont_parent_line;
	int tmp, i = 0;
	bool cont = false;
	bl = malloc(sizeof(struct block));
	bl->first = malloc(sizeof(struct line));
	bl->lenght = 0;
	curr_line = bl->first;
	curr_line->cont = NULL;
	curr_line->next = NULL;
	do {
		tmp = fgetc(fp);
		if (tmp == '\n') {
			bl->lenght++;
			curr_line->val[i] = '\0';
			if (cont){
				curr_line = cont_parent_line;
				cont = false;
			}
			curr_line->next = malloc(sizeof(struct line));
			curr_line = curr_line->next;
			curr_line->cont = NULL;
			curr_line->next = NULL;
			i = 0;
		} else if (i >= LINE_LENGHT - 1) {
			curr_line->val[i] = '\0';
			curr_line->cont = malloc(sizeof(struct line));
			if (!cont){
				cont = true;
				cont_parent_line = curr_line;
			}
			curr_line = curr_line->cont;
			curr_line->cont = NULL;
			curr_line->next = NULL;
			i = 0;
			curr_line->val[i++] = (char) tmp;	
		} else {
			curr_line->val[i++] = (char) tmp;	
		}
	} while(tmp != EOF);
	return bl;
}

void print_bl(struct block *bl, int line_num, struct text_box_start ts){
	int i;
	struct line *curr_line, *cont_line;
	struct block *curr_block;
	curr_line = bl->first;
	/* get to right block */
	for (i = 0; i < ts.block && bl->next != NULL; i++){
		curr_block = curr_block->next;
	}
	/* get to right line */
	for (i = 0; i < ts.line && curr_line->next != NULL; i++){
		curr_line = curr_line->next;
	}
	for (i = 0; i < line_num && curr_line->next != NULL; i++){
		printf("%s", curr_line);
		cont_line = curr_line->cont;
		while (cont_line != NULL){
			printf("%s", cont_line);
			cont_line = cont_line->cont;
		}
		curr_line = curr_line->next;
		putchar('\r');
		putchar('\n');
	}
}


int main(int argc, char *argv[])
{
	struct winsize w;
	int tmp;
	struct block *bl;
	struct text_box_start ts;
	struct page pg;
	bool ref; //refresh screen

	/* If no file specified, exit */
	if (argc != 2){
		fprintf(stderr, "Wrong number of args, specify a file\r\n");
		exit(1);
	}

	/* Load file */
	FILE * fp = fopen(argv[1], "r");
	if (fp == NULL){
		fprintf(stderr, "Couldn't read the file\r\n");
		exit(1);
	}
	/* Clear screen, set it raw, and get terminal size */
	clear_screen();
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	prep_term();
	atexit(restore_term);

	/* Initialize text box and cursor*/
	ts.block = 0;
	ts.line = 0;
	cursor_init();
	
	/* Load page */
	bl = load_block(fp); //TODO refactori
	pg.first = bl;

	/* Print content of block */
	print_bl(bl, w.ws_row, ts);
	cursor_print(ts, pg);

	/* 
		Get user input
		TODO add block support
	*/
	do {
		tmp = getchar();
		if (tmp == 'k'){
			cursor_mv_up(pg);
		} else if (tmp == 'j'){
			cursor_mv_down(pg);
		} 
		/*
		else if (tmp == 'l'){
			if (curs.pos >= LINE_LENGHT - 1){
				(curs.line_block)++;
				curs.pos = 0;
			} else {
				(curs.pos)++;
			}
		} else if (tmp == 'h'){
			if (curs.pos <= 0){
				(curs.line_block)--;
				curs.pos = LINE_LENGHT -1;
			} else {
				(curs.pos)--;
			}
		}
		if (curs.line - ts.line > w.ws_row){
			(ts.line)++;
		} else if (curs.line - ts.line < 0){
			(ts.line)--;
		}
		*/
		clear_screen();
		print_bl(bl, w.ws_row, ts);
		cursor_print(ts, pg);
	} while (tmp != 'q');
}