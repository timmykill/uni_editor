#include <stdint.h>

#include "utils.h"
#include "second_buffer.h"

static char buf[BUF_SIZE];
static uint_fast16_t buf_end = 0;

void buf2_put_char(char c)
{
	if (buf_end < BUF_SIZE - 1) // -1 to account for NULL char
		buf[buf_end++] = c;
	else 
		die("Buffer segfault");
}

void buf2_put_string(char* c)
{
	while(*c != '\0')
		buf2_put_char(*(c++));
}

void buf2_rm_char()
{
	if (buf_end > 0)
		buf_end--;
}

/*
	Not safe at all.
	Should be copied to another location to prevent people from changing the buffer
	For now its ok
*/
char * buf2_get_content()
{
	buf[buf_end] = '\0';
	return buf;
}

void buf2_res_buf()
{
	buf_end = 0;
}
