#ifndef _ELEMENTS_H_
#define _ELEMENTS_H_

#define LINE_LENGHT 50
#define BLOCK_SIZE 10000


struct text_box_start {
	int block;
	int line;
};

struct line {
	char * val;
	size_t s;
	struct line* cont; //kinda deprecated
	struct line* prev;
	struct line* next;
};

struct block {
	struct line *first;
	struct line *last;
	size_t lenght; //Number of lines
};

struct page {
	struct block **blk_v;
	size_t s;
};

#endif
