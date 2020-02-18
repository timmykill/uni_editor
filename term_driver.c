#include <unistd.h>
#include <termios.h>
#include <stdio.h>

#include "utils.h"
#define TIMERMAX 5
#define TIMERMIN 0
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
void itoa(int i, char* buf, size_t s)
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
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);	
}
/** update the current terminal settings to allow timed input */
void enable_timer(){
	struct termios temp;
	tcgetattr(STDIN_FILENO, &temp);
	temp.c_cc[VTIME]=TIMERMAX;
	temp.c_cc[VMIN]=TIMERMIN;
	tcsetattr(STDIN_FILENO,TCSANOW, &temp);
}
/** restore the normal raw settings of the terminal */
void disable_timer(){
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	prep_term();
}

void clear_line(unsigned int line)
{
	const int max_size = 5;
	char l_c[max_size];
	if (line > 9999)
		die("term too big");
	itoa(line, l_c, max_size);
	const char *CLEAR_LINE_ANSI0 = "\x1B[";
	const char *CLEAR_LINE_ANSI1 = ";1H\x1B[K";
	write(STDOUT_FILENO, CLEAR_LINE_ANSI0, 3);
	write(STDOUT_FILENO, l_c, max_size);
	write(STDOUT_FILENO, CLEAR_LINE_ANSI1, 7);
}

void clear_screen()
{
	const char *CLEAR_SCREEN_ANSI = "\x1B[1;1H\x1B[2J";
	write(STDOUT_FILENO, CLEAR_SCREEN_ANSI, 11);
}

void print_cursor(unsigned int x, unsigned int y)
{
	const int max_size = 5;
	char x_c[max_size];
	char y_c[max_size];
	if (x > 9999 || y > 9999)
		die("term too big");
	itoa(x+1, x_c, max_size);
	itoa(y+1, y_c, max_size);
	write(STDOUT_FILENO, "\033[", 3);
	write(STDOUT_FILENO, y_c, max_size);
	write(STDOUT_FILENO, ";", 2);
	write(STDOUT_FILENO, x_c, max_size);
	write(STDOUT_FILENO, "H", 2);
}
