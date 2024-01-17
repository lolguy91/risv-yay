#ifndef SPINLOCK_H
#define SPINLOCK_H

#include "riscv.h"

struct spinlock {
    uint32_t lock;
};

#define spinlock_init(spinlock) \
    intr_off_matched(); \
    do { \
        spinlock.lock = 0; \
    } while (0)

#define aquiere(spinlock) \
    do { \
        spinlock.lock = 1; \
        while (__sync_lock_test_and_set(spinlock.lock,1) != 0) { \
            asm volatile ("wfi"); \
        } \
        __sync_synchronize(); \
    } while (0)

#define relase(spinlock) \
    do { \
    __sync_synchronize(); \
    __sync_lock_release(&lk->locked);
    intr_on_matched();
    } while (0)

#endif