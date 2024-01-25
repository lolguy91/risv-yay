#ifndef PAGEMAP_H
#define PAGEMAP_H

#include <config.h>

#define FLAG_R (1 << 0)
#define FLAG_W (1 << 1)
#define FLAG_X (1 << 2)
#define FLAG_U (1 << 3)

char map_pages(void *vaddr, void *paddr, char flags, ABNI size);
char unmap_pages(void *vaddr,char flags, ABNI size);

char user_map_pages(ABNI pid,void *vaddr, void *paddr, char flags, ABNI size);
char user_unmap_pages(ABNI pid, void *vaddr, char flags, ABNI size);

#endif