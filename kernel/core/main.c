#include <printf.h>
#include <stdint.h>
#include <arch/shared_bindings/spm.h>
#include <arch/shared_bindings/console.h>
#include <arch/shared_bindings/kalloc.h>
#include <arch/shared_bindings/scheduler.h>

void test1() {
    while (1) {
        console_write('1');   
    }
}
void test2() {
    while (1) {
        console_write('2');   
    }
}

void main(uint64_t coreid) {
    create_task(test1, 0);
    create_task(test2, 0);
    sched_init();   

    //poweroff();
}