#ifndef _TERM_DRIVER_H_
#define _TERM_DRIVER_H_

void restore_term();
void prep_term();
void clear_screen();
/* useless function overhead, useful for brain memory tho */
void print_cursor();

#endif
