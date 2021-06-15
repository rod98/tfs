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
#include "params.h"
#include "defs.h"
#include "filetable.h"
#include "defs.h"
#include "log.h"

void ftbl_init(tfs_state *dat, uint64_t bsize) {
	if (dat) {
		//~ dat->fcnt = 0;
		//~ dat->ftbl_offset = DATA_OFFSET + bsize;
	}
}

uint64_t ftbl_devpos(tfs_state *dat, uint64_t index) {
	return 16 + dat->ftbl_offset + index * FTBL_RECSIZE;
}

uint64_t ftbl_getfcnt(tfs_state *dat) {
	//uint64_t fcnt = 0;
	//~ uint64_t i8 = 9;
	//~ pwrite(dat->devdesc, &i8, sizeof(i8), dat->ftbl_offset);
	if (dat)
		pread(dat->devdesc, &(dat->fcnt), sizeof(dat->fcnt), dat->ftbl_offset);
	
	return dat ? dat->fcnt : 0;
}

uint64_t ftbl_setfcnt(tfs_state *dat, uint64_t fcnt) {
	dat->fcnt = fcnt;
	pwrite(dat->devdesc, &(dat->fcnt), sizeof(dat->fcnt), dat->ftbl_offset);
	
	log_msg("\t\tnew size:%lu \n", dat->fcnt);
	
	return dat->fcnt;
}

uint64_t ftbl_write_start(tfs_state *dat, uint64_t ind, uint64_t value) {
	//uint64_t pos = ftbl_devpos(dat, ind) + FNAME_MAXNAME + sizeof(time_t);
	uint64_t pos = ftbl_devpos(dat, ind) + FTBL_RSTRT_OFST;
	
	/* pos should point right after the time value */
	
	pwrite(dat->devdesc, &value, sizeof(value), pos);
	
	return 0;
}

uint64_t ftbl_setfsz(tfs_state *dat, uint64_t index, size_t sz) {
	//uint64_t pos = ftbl_devpos(dat, index) + FNAME_MAXNAME + sizeof(time_t);
	uint64_t pos = ftbl_devpos(dat, index) + FTBL_RSIZE_OFST;
	
	pwrite(dat->devdesc, &sz, sizeof(sz), pos);
	return 0;
}

uint64_t ftbl_getfsz(tfs_state *dat, uint64_t index) {
	//uint64_t pos = ftbl_devpos(dat, index) + FNAME_MAXNAME + sizeof(time_t);
	uint64_t pos = ftbl_devpos(dat, index) + FTBL_RSIZE_OFST;
	uint64_t sz;
	
	pread(dat->devdesc, &sz, sizeof(sz), pos);
	
	return sz;
}

uint64_t ftbl_read_start(tfs_state *dat, uint64_t ind) {
	//uint64_t pos = ftbl_devpos(dat, ind) + FNAME_MAXNAME 
	uint64_t pos = ftbl_devpos(dat, ind) + FTBL_RSTRT_OFST;
	uint64_t res = 0;
	
	/* pos should point right after the time value */
	
	pread(dat->devdesc, &res, sizeof(res), pos);
	
	return res;
}

uint64_t ftbl_newfile(tfs_state *dat, const char *path, uint64_t index, int is_dir) {
	uint64_t fcnt = ftbl_getfcnt(dat);

	if (dat) {
		log_msg("> %s\n", __FUNCTION__);

		//~ log_msg("%d\n", __LINE__);
		log_msg("\t index: %d\n", index);
		uint64_t pos  = ftbl_devpos (dat, index);
		uint8_t  val;
		
		pread(dat->devdesc, &val, sizeof(val), pos);
		val = 0; // workaround
		
		/* later note: while -> file? */
		/* While is actually not there */
		if (!val) {
			pwrite(dat->devdesc, path + 1, strlen(path), pos);
			ftbl_setfcnt(dat, fcnt + 1);
			//~ pos += FTBL_RTIME_OFST + sizeof(time_t);
			pos += FTBL_RDIRM_OFST;
			//~ if (is_dir)
			pwrite(dat->devdesc, &is_dir, sizeof(is_dir), pos);
			
			/* Right after the time, there will be block index... */
			//pos = ftbl_devpos(dat, ftbl_get_findex(dat, path)) + FNAME_MAXNAME + 8;
			
			// TODO: somehow select the first block to be filled
			#define TODO_DONE
			#ifdef TODO_DONE
			ftbl_setfsz     (dat, index, 0);
			ftbl_write_start(dat, index, index);
			//uint64_t block_ind = ftbl_getfcnt(dat);
			//log_msg("IND: \t%lu\n", block_ind);
			//pwrite(dat->devdesc, &block_ind, sizeof(block_ind), pos);
			
			#endif
		}
		
		log_msg("< %s\n", __FUNCTION__);
	}
	
	return fcnt;
}

uint64_t ftbl_infstat (tfs_state *dat, uint64_t index, char *name, struct stat *statbuf) {
	//~ log_msg("%s() {\n", __FUNCTION__);
	int ret = 0;
	
	if (index < dat->fcnt) {
		if (dat) {
			uint64_t pos = ftbl_devpos(dat, index);

			if (name) {
				pread(dat->devdesc, name, FNAME_MAXNAME, pos);
				name[FNAME_MAXNAME] = '\0';
				
				//~ log_msg("\tindx: %s\n", index);
				//~ log_msg("\tname: %s\n", name);
			}
			
			if (statbuf) {
				time_t tm;
				int is_dir;
				int pos1 = pos, pos2 = pos;
				
				pos1 += FTBL_RTIME_OFST;
				pread(dat->devdesc, &tm, sizeof(tm), pos1);
				pos2 += FTBL_RDIRM_OFST;
				pread(dat->devdesc, &is_dir, sizeof(is_dir), pos2);
			
				statbuf->st_mtime = tm;
				statbuf->st_size  = ftbl_getfsz(dat, index);
				statbuf->st_mode  = is_dir ? S_IFDIR : S_IFREG;
			}
		}
	}
	else
		//~ if (!(name && name[0] == '/' && name[1] == '\0') )
		ret = -2;
		
	//~ else
		//~ log_msg("\twot?\n");
	
	//~ log_msg("} %s\n", __FUNCTION__);
	//~ log_msg("}\n");
	
	//~ log_msg("omg ind = %d\n", index);
	//~ log_msg("omg fcnt = %d\n", dat->fcnt);
	//~ log_msg("omg ret = %d\n", ret);
	
	return ret;
}

uint64_t ftbl_get_findex(tfs_state *dat, const char *path) {
	uint64_t fcnt;
	int      found = 0;
	uint64_t     i = 0;
	
	//~ log_msg("> %s\n", __FUNCTION__);
	if (dat) {
		fcnt = ftbl_getfcnt(dat);
		log_msg("\tfcnt: %lu \n", fcnt);
		
		for (i = 0; i < fcnt && !found; i++) {
			char rname[FNAME_MAXNAME + 1];
			
			ftbl_infstat(dat, i, rname, NULL);
			
			if (!rname[0])
				i--; /* the file table can have skips... for now at least */
			else 
				if (!strcmp(rname, path+1))
					found = 1;
			
			log_msg("\t[%lu] %s\n", i, rname);
		}
	}
	
	//~ log_msg("< %s\n", __FUNCTION__);
	
	/* becaue cycle adds ++i we add - 1*/
	return found ? i - 1 : i;
}


