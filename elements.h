#ifndef _ELEMENTS_H_
#define _ELEMENTS_H_

#define LINE_LENGHT 50
#define BLOCK_SIZE 10000

struct page {
	struct block blk_v;
	size_t s;
};

struct text_box_start {
	int block;
	int line;
};

struct block {
	struct line *first;
	struct line *last;
	size_t lenght; //Number of lines
};

struct line {
	char * val;
	size_t s;
	struct line* cont;
	struct line* next;
};

#endif
