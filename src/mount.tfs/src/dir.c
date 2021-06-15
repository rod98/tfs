#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "dir.h"
#include "log.h"
#include "defs.h"
#include "file_io.h"

char *get_last_file(char *path) {
    if (strchr(path+1, '/')) {
	int slen = strlen(path);
	int i;
	for (i = slen-1; i > 0 && path[i] != '/'; i--)
	    ;
	path = path + i;
    }

    
    return path;
}
 
// returns position in the string
char *get_next_dir(const char *dirstr, char *dirname) {
    char *next_slash = strchr(dirstr,     '/');
    int i;

    dirname[0] = '/';
    if (next_slash && next_slash[0]) {
	for (i = 1; next_slash[i] != '/'; ++i) {
	    dirname[i] = next_slash[i];
	}
	dirname[i] = '\0';

    }
    else
	next_slash = NULL;
    
    return next_slash ? next_slash + i-1 : NULL;
}

void ububl(uint64_t *list, size_t len) {
    int swp = 1;
    size_t i;
    
    while (swp) {
	swp = 0;
	for (i = 1; i < len; ++i) 
	    if (list[i] < list[i - 1]) {
		uint64_t t = list[i];
		list[i] = list[i - 1];
		list[i - 1] = t;
		swp = 1;
	    }
    }
}

uint64_t *get_file_list(tfs_state *tdata, uint64_t dir_start, uint64_t *llist_len) {
    uint64_t *list = NULL;
    
    uint64_t buf[FBLOCK_SIZE/8];
    read_block(tdata, dir_start, (uint8_t *)buf);
    *llist_len = buf[0];
    list = (uint64_t *)malloc(sizeof(uint64_t) * *llist_len);
    
    memcpy(list, buf + 1, sizeof(uint64_t) * *llist_len);
    
    ububl(list, *llist_len);
    
    
    return list;
}

void dir_append(tfs_state *tdata, uint64_t directory_index, uint64_t new_index) {
    uint64_t buf[FBLOCK_SIZE/8];
    size_t len;
    
    log_msg("APPENDING file#%llu\n", new_index);
    
    read_block(tdata, directory_index, (uint8_t *)buf);
    len = buf[0];
    
    log_msg("old length: %llu\n", len);
    
    size_t i;
    for (i = 1; i <= len; ++i)
	if (buf[i] == new_index) //file is already there. doing nothing
	    break;
	    
    if (i >= len) {
	buf[0]++;
	len = buf[0];
	log_msg("new dir length: %llu\n", len);
	
	buf[len] = new_index;
	for (i = 1; i < len; ++i)
	    if (buf[i] == buf[i+1]) {
		buf[0]--;
		len = buf[0];
		memmove(buf+i,buf+i+1,(len-i+1)*sizeof(new_index));
	    }
	write_block(tdata, directory_index, (uint8_t *)buf);
    }
}
