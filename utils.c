#include <stdlib.h>
#include <stdio.h>

#include "utils.h"

char * malloc_safe(size_t size, char* fail_string)
{
	char * tmp = malloc(size);
	if (tmp == NULL)
		die(fail_string);
	return tmp;

}


void die(char* error)
{
	perror(error);
	perror("\r\n");
	exit(EXIT_FAILURE);
}
