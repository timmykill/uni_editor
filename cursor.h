#ifndef _CURSOR_H_
#define _CURSOR_H_

void cursor_mv_down(struct page pg);
void cursor_mv_up(struct page pg);
void cursor_mv_right(struct page pg);
void cursor_print(struct text_box_start st, struct page pg);
void cursor_init();
struct text_box_start cursor_calc_new_ts(struct text_box_start ts, int rows);

#endif
