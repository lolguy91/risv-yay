#ifndef SPINLOCK_H
#define SPINLOCK_H

#include <arch.h>

struct spinlock {
    unsigned int lock;
};

#define spinlock_t struct spinlock
#define SPINLOCK_INIT {0}

#define aquiere(spinlock) \
    spinlock.lock = 1; \
    intr_off_matched(); \
    while (__sync_lock_test_and_set(&spinlock.lock,1) != 0) { \
        asm volatile ("wfi"); \
    } \
    __sync_synchronize();

#define release(spinlock) \
    __sync_synchronize(); \
    __sync_lock_release(&spinlock.lock); \
    intr_on_matched();

#endif