#include <unistd.h>
#include <termios.h>
#include <stdio.h>

static struct termios oldt;

void restore_term()
{
	/* reset term params */
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

void prep_term()
{
	/* Set terminal to get char */
	struct termios newt;
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);          
	tcsetattr( STDIN_FILENO, TCSANOW, &newt);	
}

void clear_screen()
{
	const char *CLEAR_SCREEN_ANSI = "\x1B[1;1H\x1B[2J";
	write(STDOUT_FILENO, CLEAR_SCREEN_ANSI, 12);
}

void print_cursor(unsigned int x, unsigned int y)
{
	printf("\033[%d;%dH", y, x);
}
