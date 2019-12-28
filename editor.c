#include <stdlib.h>
#include <strings.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>

#include "term_driver.h"

#define LINE_LENGHT 50

struct buffer {
	struct line* first;
	int curr; //Current line
	int lenght; //Number of lines
};

struct line {
	char val[LINE_LENGHT];
	struct line* cont;
	struct line* next;
};

struct buffer load_buffer(FILE* fp){
	struct buffer buf;
	struct line* curr_line;
	struct line* cont_parent_line;
	int tmp, i = 0;
	bool cont = false;
	buf.first = malloc(sizeof(struct line));
	buf.curr = 0;
	buf.lenght = 0;
	curr_line = buf.first;
	curr_line->cont = NULL;
	curr_line->next = NULL;
	do {
		tmp = fgetc(fp);
		if (tmp == '\n') {
			buf.lenght++;
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
	return buf;
}

void print_buf(struct buffer buf, int line_num){
	int i;
	struct line* curr_line, * cont_line;
	curr_line = buf.first;
	/* get to right line */
	for (i = 0; i < buf.curr && curr_line->next != NULL; i++){
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
	struct buffer buf;
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

	/* Load buffer */
	buf = load_buffer(fp);

	/* Print content of buffer */
	print_buf(buf, w.ws_row);

	/* Get user input */
	do {
		tmp = getchar();
		ref = false;
		if (tmp == 'k' && buf.curr > 0){
			(buf.curr)--;
			ref = true;
		} else if (tmp == 'j' && buf.curr < buf.lenght - w.ws_row){
			(buf.curr)++;
			ref = true;
		}
		if (ref)
			print_buf(buf, w.ws_row);
	} while (tmp != 'q');
	
}
