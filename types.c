#include <stdlib.h>

#include "types.h"


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
	l->next = NULL;
	l->prev = NULL;
	//* il compilatore ha fatto notare che mancava il return, aggiungendolo ed eseguendo il programma, se giochi un po con il return il programma si fotte, non so se già accadeva anche prima o è per il return aggiunto *//
	return l;
}


