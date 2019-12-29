#ifndef _CURSOR_H_
#define _CURSOR_H_

void cursor_mv_down();
void cursor_mv_up();
void cursor_mv_right();
void cursor_mv_left();
void cursor_print(struct text_box_start ts);
void cursor_init(int _rows, struct page *_pg);
struct text_box_start cursor_calc_new_ts(struct text_box_start ts);

#endif
