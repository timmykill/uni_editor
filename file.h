#ifndef _FILE_H_
#define _FILE_H_

#include <stdio.h>
#include "types.h"

struct page load_page(FILE* fp);
void save_to_file(FILE * fp, struct page pg);

#endif
