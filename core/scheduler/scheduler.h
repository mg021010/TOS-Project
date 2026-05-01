// #ifndef CORE_SCHEDULER_SCHEDULER_H
// #define CORE_SCHEDULER_SCHEDULER_H

// #include "common/queue.h"

// #define DEFAULT_RR_QUANTUM 3

// typedef enum {
//     SCHEDULER_FCFS,
//     SCHEDULER_RR
// } SchedulerAlgorithm;

// typedef struct {
//     SchedulerAlgorithm algorithm;
//     int is_configured;
//     int rr_quantum;
//     IntQueue ready_queue;
//     IntQueue waiting_queue;
// } Scheduler;

// void scheduler_init(Scheduler *scheduler);
// int scheduler_is_configured(const Scheduler *scheduler);
// const char *scheduler_algorithm_name(SchedulerAlgorithm algorithm);
// int scheduler_enqueue_ready(Scheduler *scheduler, int process_index);
// size_t scheduler_ready_count(const Scheduler *scheduler);
// const IntQueue *scheduler_ready_queue(const Scheduler *scheduler);

// #endif





#ifndef CORE_SCHEDULER_SCHEDULER_H
#define CORE_SCHEDULER_SCHEDULER_H

#include "common/queue.h"

#define DEFAULT_RR_QUANTUM 3

// 스케줄링 알고리즘 종류
typedef enum {
    SCHEDULER_FCFS,
    SCHEDULER_RR
} SchedulerAlgorithm;

// 스케줄러 상태를 관리하는 핵심 구조체
typedef struct {
    SchedulerAlgorithm algorithm;
    int is_configured;
    int rr_quantum;
    IntQueue ready_queue;
    IntQueue waiting_queue;
} Scheduler;

// 기본 제공 함수
void scheduler_init(Scheduler *scheduler);
int scheduler_is_configured(const Scheduler *scheduler);
const char *scheduler_algorithm_name(SchedulerAlgorithm algorithm);
int scheduler_enqueue_ready(Scheduler *scheduler, int process_index);
size_t scheduler_ready_count(const Scheduler *scheduler);
const IntQueue *scheduler_ready_queue(const Scheduler *scheduler);

// =========================================================
// [추가 구현] 명령어 대응 함수 원형
// =========================================================
void scheduler_set_algorithm(Scheduler *scheduler, SchedulerAlgorithm algo, int quantum);
void scheduler_run(Scheduler *scheduler, void *ptable_ptr);
void scheduler_reset(Scheduler *scheduler);
void scheduler_print_stats(const Scheduler *scheduler);

#endif