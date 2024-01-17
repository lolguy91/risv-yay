#include "riscv.h"
#include "config.h"
#include <stdbool.h>
#include <printf.h>

void switch_to(struct context *old, struct context *new);

struct process {
    bool initialized;
    unsigned char signal;
    struct context ctx;
};

struct process tasks[NPROC];
unsigned char task_stacks[NPROC][4096];
uint64_t tasqe_num = 0;
bool sched_initialized = false;
void schedule() {
    if(!sched_initialized) {
        return;
    }
    tasqe_num = (tasqe_num + 1) % NPROC;
    while(!tasks[tasqe_num].initialized ){// ensure that the process is initialized
        tasqe_num = (tasqe_num + 1) % NPROC;
    }
    if(tasks[tasqe_num].signal != 0) {
        tasks[tasqe_num].initialized = 0;
        tasks[tasqe_num].signal = 0;
        return;
    }
    printf("switch to task %d\n",tasqe_num);
    switch_to(&tasks[tasqe_num-1].ctx,&tasks[tasqe_num].ctx);
}
void create_task(void *start_routine, void *arg) {
    for(int i = 0; i < NPROC; i++) {
        if(!tasks[i].initialized) {
            tasks[i].signal = 0;
            tasks[i].initialized = true;
            tasks[i].ctx.ra = (uint64_t)start_routine;
            tasks[i].ctx.sp = (uint64_t)task_stacks[i];
            tasks[i].ctx.a0 = (uint64_t)arg;            
        }
    }
}
void kill_task() {
    tasks[tasqe_num].signal = 1;
    schedule();
}
void sched_init() {
    sched_initialized = true;
}