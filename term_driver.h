#ifndef _TERM_DRIVER_H_
#define _TERM_DRIVER_H_

/** Set term flags needed to run the editor */
void prep_term();
/** Restore previous terminal settings */
void restore_term();
/** delete a line of text from output */
void clear_line(unsigned int line);
/** delete all text from output */
void clear_screen();
/** set the cursor on the coordinates*/
void print_cursor(unsigned int x, unsigned int y);

#endif
