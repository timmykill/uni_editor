#include <unistd.h>
#include <termios.h>
#include <stdio.h>

#include "utils.h"
#define TIMERMAX 5
#define TIMERMIN 0

#define TERM_MAX_SIZE 5
#define MAX_DECIMAL 99999

/** Saves the previous terminal settings */
static struct termios oldt;
void itoa_rec(int i, char* c, size_t s)
{
	*(c+s) = i % 10 + '0';
	if (s <= 0)
		return;
	if (i/10 != 0)
		itoa_rec(i/10, c, s-1);
	else
		itoa_rec(0, c, s-1);
}

/**
	Turns an int in to the string rapresentation 
	@param i Integer to be converted
	@param buf space where the string will be placed
	@param s size of the string
*/
void itoa(unsigned int i, char* buf, size_t s)
{
	itoa_rec(i, buf, s-2);
	buf[s-1] = '\0';
}

void restore_term()
{
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

/**
	Set terminal flags
	~ICANON : Non canonical mode
	~ECHO : Dont print chars inserted
*/
void prep_term()
{
	struct termios newt;
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO); 
	newt.c_cc[VTIME]=TIMERMAX;
	newt.c_cc[VMIN]=TIMERMIN;        
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);	
}

void clear_line(unsigned int line)
{
	char l_c[TERM_MAX_SIZE];
	if (line > MAX_DECIMAL)
		die("term too big");
	itoa(line, l_c, TERM_MAX_SIZE);
	const char *CLEAR_LINE_ANSI0 = "\x1B[";
	const char *CLEAR_LINE_ANSI1 = ";1H\x1B[K";
	write(STDOUT_FILENO, CLEAR_LINE_ANSI0, 3);
	write(STDOUT_FILENO, l_c, TERM_MAX_SIZE);
	write(STDOUT_FILENO, CLEAR_LINE_ANSI1, 7);
}

void clear_screen()
{
	const char *CLEAR_SCREEN_ANSI = "\x1B[1;1H\x1B[2J";
	write(STDOUT_FILENO, CLEAR_SCREEN_ANSI, 11);
}

void print_cursor(unsigned int x, unsigned int y)
{
	char x_c[TERM_MAX_SIZE];
	char y_c[TERM_MAX_SIZE];
	if (x > MAX_DECIMAL || y > MAX_DECIMAL)
		die("term too big");
	itoa(x+1, x_c, TERM_MAX_SIZE);
	itoa(y+1, y_c, TERM_MAX_SIZE);
	write(STDOUT_FILENO, "\033[", 3);
	write(STDOUT_FILENO, y_c, TERM_MAX_SIZE);
	write(STDOUT_FILENO, ";", 2);
	write(STDOUT_FILENO, x_c, TERM_MAX_SIZE);
	write(STDOUT_FILENO, "H", 2);
}
