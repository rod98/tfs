#ifndef FREEMAP_H
#define FREEMAP_H

#define BLCK_FREE  0
#define BLCK_TAKEN 1

void fmap_read (uint8_t *map, size_t sz, uint64_t ddesc, size_t offset);
void fmap_write(uint8_t *map, size_t sz, uint64_t ddesc, size_t offset);
int  fmap_check(uint8_t *map, size_t pos);
int  fmap_set  (uint8_t *map, size_t pos, int state);

#endif
