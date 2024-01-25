#include "pagemap.h"
#include "riscv.h"
#include "printf.h"
#include <arch/shared_bindings/kalloc.h>
#include <klibc/memory.h>

pagetable_t kpagemap;

extern char* etext;
extern char* end;

void kpagemap_init(){
    kpagemap = (pagetable_t)kalloc();
    memset(kpagemap, 0, PGSIZE);

    //map the UART0
    map_pages((void*)0x10000000, (void*)0x10000000, FLAG_R | FLAG_W, PGSIZE);

    // map kernel text executable and read-only.
    map_pages((void*)KERNBASE, (void*)KERNBASE, FLAG_R | FLAG_X, (uint64_t)etext-KERNBASE);

    // map kernel data and the physical RAM we'll make use of.
    map_pages(etext, etext, FLAG_R | FLAG_W, (uint64_t)end-(uint64_t)etext);

    //asm volatile("sfence.vma zero, zero");
    //asm volatile("csrw satp, %0" : : "r" ((uint64_t)kpagemap));
    //asm volatile("sfence.vma zero, zero");
}

pte_t *walk(pagetable_t pagetable, uint64_t va, int alloc)
{   
    if(va >= MAXVA)
      return 0;

    for(int level = 2; level > 0; level--) {
          pte_t *pte = &pagetable[PX(level, va)];
          if(*pte & PTE_V) {
            pagetable = (pagetable_t)PTE2PA(*pte);
          } else {
              if(!alloc || (pagetable = (pte_t*)kalloc()) == 0)
                return 0;
              printf("pagetable : %p\n", pagetable);
              memset(pagetable, 0, PGSIZE);
              *pte = PA2PTE(pagetable) | PTE_V;
          }
    }
    return &pagetable[PX(0, va)];
}

char map_pages(void *vaddr, void *paddr, char flags, ABNI size){
    uint64_t a, last;
    uint64_t pa = (uint64_t)paddr;
    uint64_t va = (uint64_t)vaddr;
    uint64_t perm = (flags & FLAG_R) ? PTE_R : 0;
    perm |= (flags & FLAG_W) ? PTE_W : 0;
    perm |= (flags & FLAG_X) ? PTE_X : 0;
    perm |= (flags & FLAG_U) ? PTE_U : 0;

    pte_t *pte;

    if(size == 0)
      return SYS_FAIL;
    
    a = PGROUNDDOWN(va);
    last = PGROUNDDOWN(va + size - 1);
    for(;;){
        printf("map_page: va = 0x%16llx, pa = 0x%16llx, perm = 0x%16llx\r\n", a, pa, perm);
        if((pte = walk(kpagemap, a, 1)) == SYS_FAIL)
          return SYS_FAIL;
        if(*pte & PTE_V)
          return SYS_FAIL;
        *pte = PA2PTE(pa) | perm | PTE_V;
        if(a >= last)
          break;
        a += PGSIZE;
        pa += PGSIZE;
    }
    return SYS_SUCCESS;
}
char unmap_pages(void *vaddr,char flags, ABNI size){
    return SYS_SUCCESS;
}

char user_map_pages(ABNI pid, void *vaddr, void *paddr, char flags, ABNI size){
    return SYS_FAIL;
}
char user_unmap_pages(ABNI pid, void *vaddr, char flags, ABNI size){
    return SYS_FAIL;
}