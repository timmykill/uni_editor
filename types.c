#include <stdlib.h>
#include <stdint.h>

#include "types.h"


struct block * new_blk()
{
	struct block *b;
	b = (struct block*) malloc(sizeof(struct block));
	b->first = NULL;
	b->last = NULL;
	b->s = 0;
	return b;
}

struct line * new_line()
{
	struct line *l;
	l = (struct line*) malloc(sizeof(struct line));
	l->val = NULL;
	l->s = 0;
	l->next = NULL;
	l->prev = NULL;
	return l;
}


