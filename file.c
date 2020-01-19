#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "file.h"
#include "utils.h"

struct page load_page(FILE* fp)
{
	/* Creates buffers for faster computation */
	const size_t BLK_VEC_SIZE = 5;
	const size_t STRING_SIZE = 100;
	struct page pg;
	struct block * tmp_blk;
	size_t tmp_blk_size = 0;
	struct block *tmp_blk_v[BLK_VEC_SIZE];
	size_t tmp_blk_v_size = 0;
	struct block * blk_v;
	struct line * tmp_l;
	char tmp_s[STRING_SIZE];
	size_t tmp_s_size = 0;
	int tmp_c;
	
	/* init memory */
	tmp_blk = new_blk();
	tmp_l = new_line();
	tmp_blk_v[tmp_blk_v_size++] = tmp_blk;
	tmp_blk->first = tmp_l;
	
	do {
		/* load block */
		if (tmp_blk->s >= BLOCK_SIZE){
			if (tmp_blk_v_size +1>= BLK_VEC_SIZE) {
				/* TODO what to do here */
				die("Not implemented, BLK_SIZE_MAX");
			} else {
				tmp_blk->last = tmp_l;
				tmp_blk->s = tmp_blk_size;
				tmp_blk = new_blk();
				tmp_blk_v[tmp_blk_v_size++] = tmp_blk;
				tmp_blk_size = 0;
			}
		} else {
			/* TODO more error checking */
			tmp_c = fgetc(fp);
			if (tmp_s_size +1 >= STRING_SIZE){
				/*TODO what to do here */
				die("Not implemented, STRING_SIZE_MAX");
			}
			if(tmp_c == '\n'){
				/* If there's a newline, place the tmp_s content in a line, and create a new one */
				tmp_l->next = new_line();
				tmp_s[tmp_s_size++] = '\0';
				tmp_l->s = tmp_s_size;
				tmp_l->val = malloc(tmp_s_size);
				memcpy(tmp_l->val, tmp_s, tmp_s_size);
				(tmp_l->next)->prev = tmp_l;
				tmp_l = tmp_l->next;
				tmp_s_size = 0;
				tmp_blk_size++;
			} else {
				tmp_s[tmp_s_size++] = tmp_c;
			}

		}
	} while (tmp_c != EOF);
	/* we still have the last line if file didnt end with \n (i think)
	tmp_l->next = new_line();
	tmp_l->val = malloc(tmp_s_size + 1);
	memcpy(tmp_l->val, tmp_s, tmp_s_size);
	(tmp_l->next)->prev = tmp_l;
	tmp_l = tmp_l->next;
	*/
	tmp_blk->last = tmp_l;
	tmp_blk->s = tmp_blk_size;
	pg.blk_v = malloc(sizeof(struct block *) * tmp_blk_v_size);
	memcpy(pg.blk_v, tmp_blk_v, sizeof(struct block *) * tmp_blk_v_size);
	pg.s = tmp_blk_v_size;
	return pg;
}

void save_to_file(FILE * fp, struct page pg)
{
	struct line *l;
	int i;
	rewind(fp);
	for (i = 0; i < pg.s; i++) 
		for(l = pg.blk_v[i]->first; l != NULL; l = l->next)
			fprintf(fp, "%s\n", l->val);
}
