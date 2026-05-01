#ifndef CORE_SCHEDULER_SCHEDULER_H
#define CORE_SCHEDULER_SCHEDULER_H

#include "common/queue.h"

#define DEFAULT_RR_QUANTUM 3

typedef enum {
    SCHEDULER_FCFS,
    SCHEDULER_RR
} SchedulerAlgorithm;

typedef struct {
    SchedulerAlgorithm algorithm;
    int is_configured;
    int rr_quantum;
    IntQueue ready_queue;
    IntQueue waiting_queue;

    int running_proc_idx;   // 현재 실행 중인 프로세스 인덱스 (-1은 Idle)
    int current_quantum;    // 현재 프로세스가 사용한 Time Quantum
    int context_switches;   // 문맥 교환 발생 횟수
} Scheduler;

void scheduler_init(Scheduler *scheduler);
int scheduler_is_configured(const Scheduler *scheduler);
const char *scheduler_algorithm_name(SchedulerAlgorithm algorithm);
int scheduler_enqueue_ready(Scheduler *scheduler, int process_index);
size_t scheduler_ready_count(const Scheduler *scheduler);
const IntQueue *scheduler_ready_queue(const Scheduler *scheduler);

void scheduler_set_algorithm(Scheduler *scheduler, SchedulerAlgorithm algo, int quantum);
void scheduler_run(Scheduler *scheduler, void *ptable_ptr);
void scheduler_step(Scheduler *scheduler, void *ptable_ptr);
void scheduler_reset(Scheduler *scheduler);
//void scheduler_print_stats(const Scheduler *scheduler);
void scheduler_print_stats(const Scheduler *scheduler, void *ptable_ptr);

#endif