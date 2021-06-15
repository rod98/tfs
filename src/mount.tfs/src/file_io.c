#include <unistd.h>
#include "file_io.h"
#include "defs.h"

/* buf must be exactly FBLOCK_SIZE bytes */

void read_block(tfs_state *dat, uint64_t ind, uint8_t *buf) {
	if (dat && buf) {
		uint64_t pos = dat->data_offset + FBLOCK_SIZE * ind;
		
		pread (dat->devdesc, buf, FBLOCK_SIZE, pos);
	}
}

void write_block(tfs_state *dat, uint64_t ind, uint8_t *buf) {
	if (dat && buf) {
		uint64_t pos = dat->data_offset + FBLOCK_SIZE * ind;
		
		pwrite (dat->devdesc, buf, FBLOCK_SIZE, pos);
	}
}
