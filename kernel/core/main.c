#include <printf.h>
#include <stdint.h>
#include <arch/shared_bindings/spm.h>
#include <arch/shared_bindings/console.h>


void main(uint64_t coreid) {
    printf("Hello World from core #%u!\r\n", coreid);
    while(1);
}