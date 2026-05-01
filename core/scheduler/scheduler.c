// #include "core/scheduler/scheduler.h"

// void scheduler_init(Scheduler *scheduler) {
//     scheduler->algorithm = SCHEDULER_FCFS;
//     scheduler->is_configured = 0;
//     scheduler->rr_quantum = DEFAULT_RR_QUANTUM;
//     queue_init(&scheduler->ready_queue);
//     queue_init(&scheduler->waiting_queue);
// }

// int scheduler_is_configured(const Scheduler *scheduler) {
//     return scheduler->is_configured;
// }

// const char *scheduler_algorithm_name(SchedulerAlgorithm algorithm) {
//     switch (algorithm) {
//     case SCHEDULER_FCFS:
//         return "FCFS";
//     case SCHEDULER_RR:
//         return "RR";
//     default:
//         return "UNKNOWN";
//     }
// }

// int scheduler_enqueue_ready(Scheduler *scheduler, int process_index) {
//     return queue_push(&scheduler->ready_queue, process_index);
// }

// size_t scheduler_ready_count(const Scheduler *scheduler) {
//     return queue_size(&scheduler->ready_queue);
// }

// const IntQueue *scheduler_ready_queue(const Scheduler *scheduler) {
//     return &scheduler->ready_queue;
// }





#include <stdio.h>
#include "core/scheduler/scheduler.h"
#include "core/process/process.h" 

// 전역 시스템 시간
static int global_time = 0; 

void scheduler_init(Scheduler *scheduler) {
    scheduler->algorithm = SCHEDULER_FCFS;
    scheduler->is_configured = 0;
    scheduler->rr_quantum = DEFAULT_RR_QUANTUM;
    queue_init(&scheduler->ready_queue);
    queue_init(&scheduler->waiting_queue);
}

int scheduler_is_configured(const Scheduler *scheduler) {
    return scheduler->is_configured;
}

const char *scheduler_algorithm_name(SchedulerAlgorithm algorithm) {
    switch (algorithm) {
        case SCHEDULER_FCFS: return "FCFS";
        case SCHEDULER_RR:   return "RR";
        default:             return "UNKNOWN";
    }
}

int scheduler_enqueue_ready(Scheduler *scheduler, int process_index) {
    return queue_push(&scheduler->ready_queue, process_index);
}

size_t scheduler_ready_count(const Scheduler *scheduler) {
    return queue_size(&scheduler->ready_queue);
}

const IntQueue *scheduler_ready_queue(const Scheduler *scheduler) {
    return &scheduler->ready_queue;
}

// --- 추가 구현부 ---

void scheduler_set_algorithm(Scheduler *scheduler, SchedulerAlgorithm algo, int quantum) {
    scheduler->algorithm = algo;
    scheduler->rr_quantum = quantum;
    scheduler->is_configured = 1;
    printf("Scheduling algorithm set to %s.\n", scheduler_algorithm_name(algo));
}

void scheduler_run(Scheduler *scheduler, void *ptable_ptr) {
    if (!scheduler_is_configured(scheduler)) {
        printf("Error: Please set scheduling algorithm first.\n");
        return;
    }

    ProcessTable *ptable = (ProcessTable *)ptable_ptr;
    int initial_time = global_time;

    printf("================================\n");
    printf("FCFS Simulation Start\n");
    printf("================================\n");

    if (scheduler->algorithm == SCHEDULER_FCFS) {
        int proc_index;
        while (queue_pop(&scheduler->ready_queue, &proc_index) == 0) {
            PCB *p = &ptable->entries[proc_index]; // 원본 entries 필드 사용
            
            // 대기 시간 계산 및 출력 (호위 효과 증명 포인트)
            int wait_time = global_time - p->arrival_time;
            p->waiting_time = wait_time;

            printf("Time %d: [Running] %s (Work: %d, Wait: %d)\n", 
                   global_time, p->name, p->total_work_time, wait_time);
            
            // 시간 경과 및 상태 업데이트
            global_time += p->total_work_time;
            p->state = PROCESS_TERMINATED;
            p->end_time = global_time;

            printf("Time %d: [Terminated] %s\n", global_time, p->name);
        }
    }

    printf("--------------------------------\n");
    printf("Total Execution Time: %d\n", global_time - initial_time);
    printf("================================\n");
}

void scheduler_reset(Scheduler *scheduler) {
    scheduler_init(scheduler);
    global_time = 0;
    printf("Scheduler has been reset. Global time is now 0.\n");
}

void scheduler_print_stats(const Scheduler *scheduler) {
    printf("Current Algorithm: %s\n", scheduler_algorithm_name(scheduler->algorithm));
    printf("System Global Time: %d\n", global_time);
}