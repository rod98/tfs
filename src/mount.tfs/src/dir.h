#ifndef DIR_H
#define DIR_H

#include <stdint.h>
#include "params.h"

char *get_next_dir(const char *dirstr, char *dirname);
void ububl(uint64_t *list, size_t len) ;
uint64_t *get_file_list(tfs_state *tdata, uint64_t dir_start, uint64_t *llist_len);
char *get_last_file(char *path);
void dir_append(tfs_state *tdata, uint64_t directory_index, uint64_t new_index);

#endif
