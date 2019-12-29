#include <stddef.h>

size_t safe_strlen(char *s, size_t phis_size)
{
	size_t i;
	for (i = 0; i < phis_size; i++){
		if (*(s + i) == '\0')
			return i;
	}
	return i;
}
