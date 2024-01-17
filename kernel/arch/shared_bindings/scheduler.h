#ifndef SCHED_H
#define SCHED_H

void create_task(void *start_routine, void *arg);
void kill_task();
void sched_init();

#endif