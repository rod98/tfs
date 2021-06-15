#ifndef _PARAMS_H_
#define _PARAMS_H_


#define FUSE_USE_VERSION 26


#define _XOPEN_SOURCE 500

// maintain tfs state in here
#include <limits.h>
#include <stdio.h>
#include <stdint.h>
#include "tfs.h"

typedef struct _tfs_state {
    FILE    *logfile;
    //char *devpath;
    char    *devpath;
    char    *mntpath;
    int      devdesc;
    
    uint64_t fcnt;
    uint64_t data_offset; 
    uint64_t fmap_offset; /* map of free blocks */
    uint64_t ftbl_offset; /* table of names + fileinfo */
    uint64_t btbl_offset;
    
    uint8_t *fmap;
} tfs_state;

#endif
