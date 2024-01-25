#ifndef KALLOC_H
#define KALLOC_H
#include <stdbool.h>

void freerange(void *pa_start, void *pa_end);
void kfree(void *pa);
void * kalloc(void);

#endif