#include <stdint.h>
#include <time.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <fuse.h>
#include <libgen.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/fs.h>
#include "defs.h"

//~ uint64_t size_calc(int fdes) {
	//~ uint64_t bsize;
	
	//~ ioctl(fdes, BLKGETSIZE, &bsize);
	
	//~ return bsize;
//~ }

void set_magic(int fdes) {
	int magic = MAGIC_NUMBER;
	pwrite(fdes, &magic, sizeof(magic), BASE_OFFSET);
}

void set_ftbl (int fdes) {
	unsigned char buf[4096 * FTBL_RECSIZE];
	//pwrite(fdes, buf, 4096 * FTBL_RECSIZE, FTBL);
}

void set_size(int fdes) {
	uint64_t bsize;
		
	ioctl(fdes, BLKGETSIZE, &bsize);
	
	pwrite(fdes, &bsize, sizeof(bsize), SINF_OFFSET);
}

int main(int argc, char **argv) {
	int err  =  0;
	int fdes = -1;
	
	if (argc > 1) {
		int i;
		for (i = 1; i < argc; ++i) 
			if (argv[i][0] != '-') {
				printf("Formatting %s...\n", argv[i]);
				fdes = open(argv[i], O_RDWR);
			}
	}
	else
		err = -1;
		
	if (fdes >= 0) {
		set_magic(fdes);
		set_size (fdes);
		set_ftbl (fdes);
	}
	else
		err = -2;
		
	if (err)
		fprintf(stderr, "E: %d\n", err);
	
	return err;
}
