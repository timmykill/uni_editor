#include "elements.h"
#include <stdio.h>

static struct cursor curs;

void cursor_init()
{
	curs.block = 0;
	curs.line = 0;
	curs.line_block = 0;
	curs.pos = 0;
}

void cursor_print(struct text_box_start ts, struct page pg)
{
	int x, y;
	y = curs.line - ts.line;
	printf("\033[%d;%dH", y, 0);
}

void cursor_mv_up(struct page pg)
{
	struct block *blk;
	struct line *l;
	int i = 0;
	blk = pg.first;
	l = blk->first;
	if (curs.line == 0){
		(curs.block)--;
		/* get to last line in block */
		for(; l->next != NULL; i++, l = l->next);
		curs.line = i;
	} else {
		(curs.line)--;
	}
}

void cursor_mv_down(struct page pg)
{
	struct block *blk;
	struct line *l;
	int i = 0;
	blk = pg.first;
	l = blk->first;
	/* get to last line in block */
	for(; l->next != NULL; i++, l = l->next);
	if(curs.line == i){
		curs.line = 0;
		(curs.block)++;
	} else {
		(curs.line)++;
	}
}
