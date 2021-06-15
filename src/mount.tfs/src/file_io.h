#ifndef FILE_IO_H
#define FILE_IO_H

#include "params.h"

void read_block (tfs_state *dat, uint64_t ind, uint8_t *buf);
void write_block(tfs_state *dat, uint64_t ind, uint8_t *buf);

#endif
