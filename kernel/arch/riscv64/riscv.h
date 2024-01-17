#ifndef RISCV_H
#define RISCV_H

#include <stdint.h>
#include <config.h>

typedef uint64_t reg_t;

struct cpu {
    uint64_t nintr;
};
struct context {
    reg_t ra;
    reg_t sp;
    reg_t gp;
    reg_t t0;
    reg_t t1;
    reg_t t2;
    reg_t s0;
    reg_t s1;
    reg_t a0;
    reg_t a1;
    reg_t a2;
    reg_t a3;
    reg_t a44;
    reg_t a52;
    reg_t a60;
    reg_t a78;
    reg_t s26;
    reg_t s34;
    reg_t s42;
    reg_t s50;
    reg_t s68;
    reg_t s76;
    reg_t s84;
    reg_t s92;
    reg_t s100;
    reg_t s118;
    reg_t t36;
    reg_t t44;
    reg_t t52;
    reg_t t60;
};

#ifdef _FILE_CONTAINS_RISCV_IMPL
    struct cpu cpus[NCPU];
#else
    extern struct cpu cpus[NCPU];
#endif

#define MSTATUS_MPP_MASK (3L << 11) // previous mode.
#define MSTATUS_MPP_M (3L << 11)
#define MSTATUS_MPP_S (1L << 11)
#define MSTATUS_MPP_U (0L << 11)
#define MSTATUS_MIE (1L << 3)    // machine-mode interrupt enable.


#define SSTATUS_SPP (1L << 8)  // Previous mode, 1=Supervisor, 0=User
#define SSTATUS_SPIE (1L << 5) // Supervisor Previous Interrupt Enable
#define SSTATUS_UPIE (1L << 4) // User Previous Interrupt Enable
#define SSTATUS_SIE (1L << 1)  // Supervisor Interrupt Enable
#define SSTATUS_UIE (1L << 0)  // User Interrupt Enable


// Supervisor Interrupt Enable
#define SIE_SEIE (1L << 9) // external
#define SIE_STIE (1L << 5) // timer
#define SIE_SSIE (1L << 1) // software

// Machine-mode Interrupt Enable
#define MIE_MEIE (1L << 11) // external
#define MIE_MTIE (1L << 7)  // timer
#define MIE_MSIE (1L << 3)  // software

// use riscv's sv39 page table scheme.
#define SATP_SV39 (8L << 60)

#define MAKE_SATP(pagetable) (SATP_SV39 | (((uint64_t)pagetable) >> 12))

typedef uint64_t pte_t;
typedef uint64_t *pagetable_t; // 512 PTEs

#define PGSIZE 4096
#define PGSHIFT 12 

#define PGROUNDUP(sz)  (((sz)+PGSIZE-1) & ~(PGSIZE-1))
#define PGROUNDDOWN(a) (((a)) & ~(PGSIZE-1))

#define PTE_V (1L << 0)
#define PTE_R (1L << 1)
#define PTE_W (1L << 2)
#define PTE_X (1L << 3)
#define PTE_U (1L << 4) 

#define PA2PTE(pa) ((((uint64_t)pa) >> 12) << 10)

#define PTE2PA(pte) (((pte) >> 10) << 12)

#define PTE_FLAGS(pte) ((pte) & 0x3FF)

#define PXMASK          0x1FF
#define PXSHIFT(level)  (PGSHIFT+(9*(level)))
#define PX(level, va) ((((uint64_t) (va)) >> PXSHIFT(level)) & PXMASK)

#define MAXVA (1L << (9 + 9 + 9 + 12 - 1))

#define KERNBASE 0x80000000L
#define PHYSTOP (KERNBASE + 128*1024*1024)

#define r_sstatus(x) \
    do { \
        asm volatile ("csrr %0, sstatus" : "=r" (x) ); \
    } while (0);

#define w_sstatus(x) \
    do { \
        asm volatile ("csrw sstatus, %0" : : "r" (x) ); \
    } while (0);

#define r_tp(x) \
    do { \
        asm volatile ("mv %0, tp" : "=r" (x) ); \
    } while (0);

static inline void intr_off() {
    uint64_t x; \
    r_sstatus(x); \
    x &= ~SSTATUS_SIE; \
    w_sstatus(x); 
    
}

static inline void intr_on() {
    uint64_t x; \
    r_sstatus(x); \
    x |= SSTATUS_SIE; \
    w_sstatus(x); 
}
static inline void intr_off_matched() {
    uint64_t hartid; \
    r_tp(hartid); \
    cpus[hartid].nintr += 1; \
    intr_off();
}
static inline void intr_on_matched() {
    uint64_t hartid; \
    r_tp(hartid); \
    cpus[hartid].nintr -= 1; \
    if(cpus[hartid].nintr == 0) \
        intr_on(); 
}
#endif