#ifndef _ELEMENTS_H_
#define _ELEMENTS_H_

#define LINE_LENGHT 50
#define BLOCK_SIZE 10000

struct page {
	struct block *first;
};

struct cursor {
	int block;
	int line;
	int line_block;
	int pos; //position inside the line block
};

struct text_box_start {
	int block;
	int line;
};

struct block {
	struct line *first;
	struct line *last;
	int lenght; //Number of lines
	struct block *next;
};

struct line {
	char val[LINE_LENGHT];
	struct line* cont;
	struct line* next;
};

#endif
