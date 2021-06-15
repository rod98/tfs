#ifndef FILETABLE_H
#define FILETABLE_H

#include "params.h"

void     ftbl_init      (tfs_state *dat, uint64_t bsize);
uint64_t ftbl_newfile   (tfs_state *dat, const char *path, uint64_t index, int is_dir);
uint64_t ftbl_getfcnt   (tfs_state *dat);
uint64_t ftbl_get_findex(tfs_state *dat, const char *path);
uint64_t ftbl_devpos    (tfs_state *dat, uint64_t index);
uint64_t ftbl_infstat   (tfs_state *dat, uint64_t index, char *name, struct stat *statbuf);
uint64_t ftbl_read_start(tfs_state *dat, uint64_t ind);
uint64_t ftbl_getfsz    (tfs_state *dat, uint64_t ind);
uint64_t ftbl_setfsz    (tfs_state *dat, uint64_t ind, size_t sz);

#endif
