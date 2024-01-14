#include <stdint.h>
#include "plic.h"
#include "riscv.h"
#include "uart.h"
#include "clint.h"

__attribute__ ((aligned (16))) char stack0[1024*2];
uint64_t timer_scratch[4][5];


#define MSTATUS_MPP_MASK (3L << 11) // previous mode.
#define MSTATUS_MPP_M (3L << 11)
#define MSTATUS_MPP_S (1L << 11)
#define MSTATUS_MPP_U (0L << 11)


void main(uint64_t coreid);
void _start_supervirsor();

extern void kernelvec();
extern void timervec();

void start(){
    uint64_t coreid;
    asm volatile("csrr %0, mhartid" : "=r" (coreid) );
    asm volatile("mv tp, %0" : : "r" (coreid));

    uint64_t mstatus;
    asm volatile("csrr %0, mstatus" : "=r" (mstatus) );
    mstatus &= ~MSTATUS_MPP_MASK;
    mstatus |= MSTATUS_MPP_S;
    asm volatile("csrw mstatus, %0" : : "r" (mstatus));

    asm volatile("csrw mepc, %0" : : "r" ((uint64_t)_start_supervirsor));
    asm volatile("csrw satp, %0" : : "r" (0));

    asm volatile("csrw medeleg, %0" : : "r" (0xffff));
    asm volatile("csrw mideleg, %0" : : "r" (0xffff));

    asm volatile("csrw pmpaddr0, %0" : : "r" (0x3fffffffffffffull));
    asm volatile("csrw pmpcfg0, %0" : : "r" (0xf));

    int interval = 1000000;
    *(uint64_t*)CLINT_MTIMECMP(coreid) = *(uint64_t*)CLINT_MTIME + interval;
    uint64_t *scratch = &timer_scratch[coreid][0];
    scratch[3] = CLINT_MTIMECMP(coreid);
    scratch[4] = interval;
    asm volatile("csrw mscratch, %0" : : "r" (scratch));
    asm volatile("csrw mtvec, %0" : : "r" ((uint64_t)timervec));
    asm volatile("csrw mstatus, %0" : : "r" (mstatus | MSTATUS_MIE));
    uint64_t mie;
    asm volatile("csrr %0, mie" : "=r" (mie) );
    asm volatile("csrw mie, %0" : : "r" (mie | MIE_MTIE));
    asm volatile("mret");
}

void _start_supervirsor()
{
    uint64_t coreid;
    asm volatile("mv %0, tp" : "=r" (coreid) );

    uint64_t sie;
    asm volatile("csrr %0, sie" : "=r" (sie) );
    asm volatile("csrw sie, %0" : : "r" (sie | SIE_SEIE | SIE_STIE | SIE_SSIE));

    asm volatile("csrw stvec, %0" : : "r" ((uint64_t)kernelvec));//register interrupt handler


    if (coreid == 0) {
        plic_init();
        plic_init_hart(coreid);
        init_uart();
    } else{
        plic_init_hart(coreid);
    }

    main(coreid);
}