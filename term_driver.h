#ifndef _TERM_DRIVER_H_
#define _TERM_DRIVER_H_

#include <termios.h>

void restore_term();
void prep_term();
void clear_screen();

#endif
