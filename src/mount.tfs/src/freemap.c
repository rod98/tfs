#include <stdint.h>
#include <stddef.h>
#include <unistd.h>
#include "freemap.h"

void fmap_read (uint8_t *map, size_t sz, uint64_t ddesc, size_t offset) {
	if (map) 
		pread(ddesc, map, sz, offset);
}

void fmap_write(uint8_t *map, size_t sz, uint64_t ddesc, size_t offset) {
	if (map)
		pwrite(ddesc, map, sz, offset);
}

int fmap_check(uint8_t *map, size_t pos) {
	size_t byt_pos = pos / 8; /* get to the byte that contains our bit */
	size_t bit_pos = pos % 8; /* find the bit in that byte */
	
	return map[byt_pos] & (1 << bit_pos) ? BLCK_TAKEN : BLCK_FREE;
}

int fmap_set(uint8_t *map, size_t pos, int state) {
	size_t byt_pos = pos / 8; /* get to the byte that contains our bit */
	size_t bit_pos = pos % 8; /* find the bit in that byte */
	
	if (state == BLCK_TAKEN)
		map[byt_pos] |= (1 << bit_pos);
	else
		map[byt_pos] &= (~(1 << bit_pos));
	
	return fmap_check(map, pos);
}
