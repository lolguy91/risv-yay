#define _FILE_CONTAINS_RISCV_IMPL

#include <stdint.h>
#include "plic.h"
#include "riscv.h"
#include "uart.h"
#include "clint.h"
#include "printf.h"
#include "config.h"

__attribute__ ((aligned (16))) char stack0[4096*NCPU];
uint64_t timer_scratch[NCPU][5];

void main(uint64_t coreid);
void _start_supervirsor();

extern void kernelvec();
extern void timervec();

void start(){
    uint64_t coreid;
    asm volatile("csrr %0, mhartid" : "=r" (coreid) );
    asm volatile("mv tp, %0" : : "r" (coreid));

    //initialize MSTATUS and SSTATUS
    uint64_t mstatus;
    uint64_t sstatus;
    asm volatile("csrr %0, mstatus" : "=r" (mstatus) );
    r_sstatus(sstatus);
    mstatus &= ~MSTATUS_MPP_MASK;
    mstatus |= MSTATUS_MPP_S;
    mstatus |= MSTATUS_MIE;
    sstatus |= SSTATUS_SIE;
    asm volatile("csrw mstatus, %0" : : "r" (mstatus));
    w_sstatus(sstatus);

    //disable paging
    asm volatile("csrw satp, %0" : : "r" (0));

    //delegate all interrupts to supervisor
    asm volatile("csrw medeleg, %0" : : "r" (0xffff));
    asm volatile("csrw mideleg, %0" : : "r" (0xffff));

    //set pmp
    asm volatile("csrw pmpcfg0, %0" : : "r" (0));
    asm volatile("csrw pmpaddr0, %0" : : "r" (0x3fffffffffffffull));
    asm volatile("csrw pmpcfg0, %0" : : "r" (0xf));
    
    //enable interrupts
    uint64_t sie;
    asm volatile("csrr %0, sie" : "=r" (sie) );
    asm volatile("csrw sie, %0" : : "r" (sie | SIE_SEIE | SIE_STIE | SIE_SSIE));
    asm volatile("csrw stvec, %0" : : "r" ((uint64_t)kernelvec));//register interrupt handler

    //set up timer
    int interval = 1000000;
    *(uint64_t*)CLINT_MTIMECMP(coreid) = *(uint64_t*)CLINT_MTIME + interval;
    uint64_t *scratch = &timer_scratch[coreid][0];
    scratch[3] = CLINT_MTIMECMP(coreid);
    scratch[4] = interval;
    asm volatile("csrw mscratch, %0" : : "r" (scratch));
    asm volatile("csrw mtvec, %0" : : "r" ((uint64_t)timervec));
    uint64_t mie;
    asm volatile("csrr %0, mie" : "=r" (mie) );
    asm volatile("csrw mie, %0" : : "r" (mie | MIE_MTIE));

    //set previous code address to _start_supervirsor
    asm volatile("csrw mepc, %0" : : "r" ((uint64_t)_start_supervirsor));
    
    //get into supervisor mode
    asm volatile("mret");
}

void kalloc_init();
void kpagemap_init();

volatile static int started = 0;

void _start_supervirsor()
{
    uint64_t coreid;
    r_tp(coreid);

    if (coreid > NCPU) {
        // maximum cores exceeded
        while (1) {
            asm volatile("wfi");
        }
    }
    if (coreid == 0) {
        printf("Da kernel is starting!\r\n");
        init_uart();
        kalloc_init();
        //kpagemap_init();
        started = 1;
    }else{
        while (started == 0) {
            asm volatile("wfi");
        }
        printf("Core %d is starting!\r\n", coreid);
    }
    main(coreid);
}
