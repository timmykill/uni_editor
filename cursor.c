#include <stdio.h>
#include <stdbool.h>

#include "elements.h"
#include "strings.h"

static int held_x_pos;
static bool holding_x;
static int x_pos;
static int y_pos;
static int rows;
static bool up_ts;
static bool down_ts;
static struct page *pg;


/* NEVER MODIFY THE PAGE FROM HERE */
void cursor_init(int _rows, struct page *_pg)
{
	held_x_pos = 0;
	x_pos = 0;
	y_pos = 0;
	rows = _rows;
	up_ts = false;
	down_ts = false;
	pg = _pg;
	holding_x = false;
}

struct text_box_start cursor_calc_new_ts(struct text_box_start ts)
{
	struct block *blk;
	int file_lines, i;

	blk = pg->first;

	if (down_ts) {
		/*go to right block */
		for(i = 0; i < ts.block; i++, blk = blk->next);
		if (ts.line < (blk->lenght - rows)){
			(ts.line)++;
		} else if (ts.line == blk->lenght - rows && blk->next != NULL) {
			(ts.block)++;
			(ts.line) = 0;
		}	
		down_ts = false;
	} else if (up_ts){
		if (ts.line > 0){
			(ts.line)--;
		} else if (ts.line == 0 && ts.block > 0){
			(ts.block)--;
			/*go to right block */
			for(i = 0; i < ts.block; i++, blk = blk->next)
			ts.line = blk->lenght;
		}
		up_ts = false;
	}
	return ts;
}



void cursor_print(struct text_box_start ts)
{
	int i, s_len;	
	struct block *blk;
	struct line *l;
	
	blk = pg->first;
	
	/*go to right block */
	for(i = 0; i < ts.block; i++, blk = blk->next);
		
	l = blk->first;
	/*go to right line */
	for(i = 0; i < ts.line; i++, l = l->next);
	for(i = 0; i < y_pos; i++, l = l->next);
	
	// line cont

	s_len = safe_strlen(l->val, LINE_LENGHT);
	
	/*
	 * absolute mess 
	 *	TODO sistemare
	 * */
	if (x_pos > s_len) {
		x_pos = s_len;
		holding_x = true;
	} else if (holding_x){
		if (held_x_pos > s_len){
			x_pos = held_x_pos;
			holding_x = false;
		} else {
			x_pos = s_len;
		}
	} else {
		holding_x = false;
	}
	if (x_pos > held_x_pos)
		held_x_pos = x_pos;
		
	printf("\033[%d;%dH", y_pos, x_pos);
}

void cursor_mv_up()
{
	if (y_pos > 0)
		y_pos--;
	else
		up_ts = true;
}

void cursor_mv_down()
{
	if(y_pos < rows)
		y_pos++;
	else
		down_ts = true;
}

void cursor_mv_right()
{
	x_pos++;
}

void cursor_mv_left()
{
	x_pos--;
}
