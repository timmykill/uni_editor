# Ricerche effettuate per realizzare l'editor

Può essere utile in caso voglia creare delle anki card

## How do you clear the console screen in C?
``` c
printf("\e[1;1H\e[2J");
```

This function will work on ANSI terminals, demands POSIX. I assume there is a version that might also work on window's console, since it also supports ANSI escape sequences.

``` c
#include <unistd.h>

void clearScreen()
{
	const char *CLEAR_SCREEN_ANSI = "\e[1;1H\e[2J";
	write(STDOUT_FILENO, CLEAR_SCREEN_ANSI, 12);
}
```
There are some other alternatives, some of which don't move the cursor to {1,1}.

## Come sapere la dimensione del terminale
``` c
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>

int main (int argc, char **argv)
{
    struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

	printf ("lines %d\n", w.ws_row);
	printf ("columns %d\n", w.ws_col);
	return 0;  // make sure your main returns int
}
```

## Inizializzare puntatore in struct

Non si possono inizializzare puntatori direttamente nello struct, bisogna farlo dopo la malloc

## How to avoid pressing enter with getchar()

``` c
static struct termios oldt, newt;

/*
	tcgetattr gets the parameters of the current terminal
	STDIN_FILENO will tell tcgetattr that it should write the settings
	of stdin to oldt
*/
tcgetattr( STDIN_FILENO, &oldt);

/* now the settings will be copied */
newt = oldt;

/*
	ICANON normally takes care that one line at a time will be processed
	that means it will return if it sees a "\n" or an EOF or an EOL
*/
newt.c_lflag &= ~(ICANON);          

/*
	Those new settings will be set to STDIN
	TCSANOW tells tcsetattr to change attributes immediately.
*/
tcsetattr( STDIN_FILENO, TCSANOW, &newt);

/*restore the old settings*/
tcsetattr( STDIN_FILENO, TCSANOW, &oldt);

```

## Unexpected indentation behaviour when I set the terminal to raw mode – why is this happening?

One of the stty settings (onlcr) tells the terminal driver to convert newline (which is actually ASCII line-feed) to carriage-return plus line-feed.

Unix-like systems just write a newline to end lines, letting the terminal driver do the right thing (convert newline to carriage-return plus line-feed).

Carriage-return "goes left" and line-feed "goes down".

When you set the terminal to raw mode, newline will no longer be converted to carriage-return plus line-feed. Lacking the carriage-returns, you get that staircase effect.

