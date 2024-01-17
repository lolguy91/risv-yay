#include "plic.h"
#include "uart.h"
#include "riscv.h"
#include "printf.h"

extern void schedule();

void clockinterrupt(){
    schedule();
}

void handler(){
    
    uint64_t coreid;
    uint64_t sepc;
    uint64_t sstatus;
    uint64_t scause;

    r_tp(coreid);
    asm volatile("csrr %0, sepc" : "=r" (sepc));
    r_sstatus(sstatus);
    asm volatile("csrr %0, scause" : "=r" (scause));

    if((scause & 0x8000000000000000L) &&
        (scause & 0xff) == 9){
        //uint64_t irq = plic_claim(coreid);
        //plic_complete(irq,coreid);

    }else if(scause == 0x8000000000000001L){
        clockinterrupt();
        uint64_t sip;
        asm volatile("csrr %0, sip" : "=r" (sip) );
        sip &= ~(1 << 2);
        asm volatile("csrw sip, %0" : : "r" (sip));
    }

    asm volatile("csrw sepc, %0" : : "r" (sepc));
    w_sstatus(sstatus);
}
