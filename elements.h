#ifndef _ELEMENTS_H_
#define _ELEMENTS_H_

#define LINE_LENGHT 50
#define BLOCK_SIZE 10000


struct page_coord {
	int block;
	int line;
};

struct line {
	char * val;
	size_t s;
	struct line* prev;
	struct line* next;
};

struct block {
	struct line *first;
	struct line *last;
	size_t s; //Number of lines
};

struct page {
	struct block **blk_v;
	size_t s;
};

struct cursor {
	unsigned int x;
	unsigned int y;
};

#endif
