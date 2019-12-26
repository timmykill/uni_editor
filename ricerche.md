#Ricerche effettuate per realizzare l'editor, può essere utile in caso voglia creare delle anki card

## How do you clear the console screen in C?
```
printf("\e[1;1H\e[2J");
```

This function will work on ANSI terminals, demands POSIX. I assume there is a version that might also work on window's console, since it also supports ANSI escape sequences.

```
#include <unistd.h>

void clearScreen()
{
  const char *CLEAR_SCREEN_ANSI = "\e[1;1H\e[2J";
    write(STDOUT_FILENO, CLEAR_SCREEN_ANSI, 12);
	}
```
There are some other alternatives, some of which don't move the cursor to {1,1}.

