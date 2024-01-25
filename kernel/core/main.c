#include <printf.h>
#include <stdint.h>
#include <arch/shared_bindings/spm.h>
#include <arch/shared_bindings/console.h>
#include <arch/shared_bindings/kalloc.h>

void main(uint64_t coreid) {
    printf("Hello World from core #%u!\r\n", coreid);
    //kalloc test
    char *ptr = kalloc();
    printf("kalloc: %p\r\n", ptr);
    *ptr = 0x42;
    printf("the meaning of life: 0x%x\r\n", *ptr);
    kfree(ptr);
    poweroff();
}