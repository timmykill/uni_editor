#include <stdio.h>

#include "elements.h"
#include "strings.h"

static struct cursor curs;
static int held_x_pos;

void cursor_init()
{
	curs.block = 0;
	curs.line = 0;
	curs.line_block = 0;
	curs.pos = 0;
}

struct text_box_start cursor_calc_new_ts(struct text_box_start ts, int rows)
{
	
	if (curs.line - ts.line > rows)
		(ts.line)++;
	else if (curs.line - ts.line < 0)
		(ts.line)--;
	return ts;
}

void cursor_print(struct text_box_start ts, struct page pg)
{
	unsigned int x, y;
	y = curs.line - ts.line;
	x = curs.line_block * LINE_LENGHT + curs.pos;
	printf("\033[%d;%dH", y, x);
}

void cursor_mv_up(struct page pg)
{
	struct block *blk;
	int i = 0;
	blk = pg.first;
	if (curs.line == 0 && curs.block > 0){
		(curs.block)--;
		curs.line = blk->lenght;
	} else if (curs.line > 0) {
		(curs.line)--;
	}
}

void cursor_mv_down(struct page pg)
{
	struct block *blk;
	blk = pg.first;
	if(curs.line == blk->lenght && blk->next != NULL){
		curs.line = 0;
		(curs.block)++;
	} else if (curs.line < blk->lenght){
		(curs.line)++;
	}
}

void cursor_mv_right(struct page pg)
{
	struct block *blk;
	struct line *l;
	int i, l_len;
	blk = pg.first;
	l = blk->first;
	for(i = 0; i <= curs.line && l->next != NULL; i++, l = l->next);
	for(i = 0; i <= curs.line_block && l->cont != NULL; i++, l = l->cont);
	/* l is now the right line block */
	l_len = safe_strlen(l->val, LINE_LENGHT);
	
	if (curs.pos >= l_len && l->cont != NULL){
		curs.pos = 0;
		(curs.line_block)++;
	} else if (curs.pos < l_len){
		(curs.pos)++;
	}
}
