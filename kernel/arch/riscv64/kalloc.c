#include "riscv.h"
#include "config.h"
#include "kalloc.h"
#include "printf.h"
#include <klibc/memory.h>

void freerange(void *pa_start, void *pa_end);
extern char* end;

#define BITMAP_SIZE (MAX_MEMORY / PGSIZE)

uint64_t bitmap[BITMAP_SIZE / 64]; // Assuming each bitmap bit represents a single page

void set_bit(uint64_t *bitmap, int index) {
    bitmap[index / 64] |= 1ULL << (index % 64);
}

void clear_bit(uint64_t *bitmap, int index) {
    bitmap[index / 64] &= ~(1ULL << (index % 64));
}

int get_bit(uint64_t *bitmap, int index) {
    return (bitmap[index / 64] & (1ULL << (index % 64))) != 0;
}

void init_bitmap() {
    memset (bitmap, 0, sizeof(bitmap));
}

void kalloc_init()
{
  init_bitmap();
  freerange(end,end + MAX_MEMORY/PGSIZE);
  kalloc(); // horrible fix for a nullptr issue
}


void freerange(void *pa_start, void *pa_end) {
    uint64_t start_index = ((uint64_t)pa_start - (uint64_t)end) / PGSIZE;
    uint64_t end_index = ((uint64_t)pa_end - (uint64_t)end) / PGSIZE;
    
    for (uint64_t i = start_index; i < end_index; i++) {
        set_bit(bitmap, i);
    }
}
void kfree(void *pa) {
    // Calculate the page index from the given physical address
    uint64_t index = ((uint64_t)pa - (uint64_t)end) / PGSIZE;
    // Set the bit corresponding to the page in the bitmap to mark it as free
    set_bit(bitmap, index);
}
void* kalloc(void)
{
    for (uint64_t i = (uint64_t)end; i < (uint64_t)end + BITMAP_SIZE; i++) {
        if (get_bit(bitmap, i)) {
            clear_bit(bitmap, i);
            return (void*)(end + i * PGSIZE);
        }
    }
    return 0;
}
