#ifndef _MYTYPES_H_
#define _MYTYPES_H_

#include <stdint.h>

#define LINE_LENGHT 50
#define BLOCK_SIZE 10000


/**
	Not used yet, its a pointer to a certain location in memory,
	using block / line hierarchy
*/
struct page_coord {
	uint_fast16_t block;
	uint_fast16_t line;
};

/**
	Doubly linked list, contains a block of text in form of a char array
*/
struct line {
	char * val;
	size_t s;
	struct line* prev;
	struct line* next;
};

/**
	Block of lines, contains the first and last line of the list
*/
struct block {
	struct line *first;
	struct line *last;
	size_t s; //Number of lines
};

/**
	A page contains n blocks of text
*/
struct page {
	struct block **blk_v;
	size_t s;
};

/** Block constuctor */
struct block * new_blk();
/** Line constuctor */
struct line * new_line();

#endif
