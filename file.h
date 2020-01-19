#ifndef _FILE_H_
#define _FILE_H_

#include <stdio.h>
#include "types.h"

/** Load file in to page struct */
struct page load_page(FILE* fp);
/** Save page struct in to file */
void save_to_file(FILE * fp, struct page pg);

#endif
