#include <stdlib.h>
#include <strings.h>
#include <stdio.h>


struct line {
	char[10] a;
	line* ext;
	line* next;
}



int main()
{
	/* Load file */
	FILE fp = fread("test.txt", rw);
	if (fp == NULL)
		exit(1);
	


	char c;
	while (c != 'e'){
		
	}
}
