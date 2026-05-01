// #include <stdio.h>
// #include "core/scheduler/scheduler.h"
// #include "core/process/process.h" 

// static int global_time = 0; 

// static void visualize_system_state(Scheduler *scheduler, ProcessTable *ptable) {
//     printf("\nAlgorithm: %s\n", scheduler_algorithm_name(scheduler->algorithm));
//     if (scheduler->algorithm == SCHEDULER_RR) {
//         printf("Quantum: %d\n", scheduler->rr_quantum);
//     }
    
//     if (scheduler->running_proc_idx != -1) {
//         printf("Running: %s\n", ptable->entries[scheduler->running_proc_idx].name);
//     } else {
//         printf("Running: None\n");
//     }

//     printf("Ready Queue: %zu processes\n", queue_size(&scheduler->ready_queue));
//     printf("Waiting Queue: %zu processes\n", queue_size(&scheduler->waiting_queue));

//     printf("Terminated: ");
//     int found_term = 0;
//     for (int i = 0; i < ptable->count; i++) {
//         if (ptable->entries[i].state == PROCESS_TERMINATED) {
//             printf("%s ", ptable->entries[i].name);
//             found_term = 1;
//         }
//     }
//     if (!found_term) printf("Empty");

//     printf("\nContext Switches: %d\n", scheduler->context_switches);
//     printf("Global Time: %d\n\n", global_time);
// }

// void scheduler_init(Scheduler *scheduler) {
//     scheduler->algorithm = SCHEDULER_FCFS;
//     scheduler->is_configured = 0;
//     scheduler->rr_quantum = DEFAULT_RR_QUANTUM;
//     scheduler->running_proc_idx = -1;
//     scheduler->current_quantum = 0;
//     scheduler->context_switches = 0;
//     queue_init(&scheduler->ready_queue);
//     queue_init(&scheduler->waiting_queue);
// }

// int scheduler_is_configured(const Scheduler *scheduler) {
//     return scheduler->is_configured;
// }

// const char *scheduler_algorithm_name(SchedulerAlgorithm algorithm) {
//     switch (algorithm) {
//         case SCHEDULER_FCFS: return "FCFS";
//         case SCHEDULER_RR:   return "RR";
//         default:             return "UNKNOWN";
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

// void scheduler_set_algorithm(Scheduler *scheduler, SchedulerAlgorithm algo, int quantum) {
//     scheduler->algorithm = algo;
//     scheduler->rr_quantum = quantum;
//     scheduler->is_configured = 1;
//     printf("Scheduling algorithm set to %s.\n", scheduler_algorithm_name(algo));
// }

// void scheduler_step(Scheduler *scheduler, void *ptable_ptr) {
//     ProcessTable *ptable = (ProcessTable *)ptable_ptr;

//     // 1. Waiting Queue 처리 (I/O 진행)
//     int waiting_count = queue_size(&scheduler->waiting_queue);
//     for (int i = 0; i < waiting_count; i++) {
//         int w_idx;
//         queue_pop(&scheduler->waiting_queue, &w_idx);
//         PCB *wp = &ptable->entries[w_idx];
//         wp->io_remaining--;
//         if (wp->io_remaining <= 0) {
//             wp->state = PROCESS_READY;
//             scheduler_enqueue_ready(scheduler, w_idx);
//             printf("Time %d: [I/O Completed] %s back to Ready Queue\n", global_time, wp->name);
//         } else {
//             queue_push(&scheduler->waiting_queue, w_idx);
//         }
//     }

//     // 2. CPU가 비어있다면 지금 즉시 채우기
//     if (scheduler->running_proc_idx == -1) {
//         if (queue_pop(&scheduler->ready_queue, &scheduler->running_proc_idx) == 0) {
//             PCB *p = &ptable->entries[scheduler->running_proc_idx];
//             p->state = PROCESS_RUNNING;
//             if (p->executed_time == 0) p->start_time = global_time;
//             scheduler->current_quantum = 0;
//             scheduler->context_switches++;
//         }
//     }

//     // 3. 프로세스 실행 및 시간 증가
//     if (scheduler->running_proc_idx != -1) {
//         PCB *p = &ptable->entries[scheduler->running_proc_idx];
//         p->executed_time++;
//         p->remaining_time--;
//         global_time++; // 여기서 시간이 흐릅니다.
//         scheduler->current_quantum++;

//         // -------------------------------------------------------
//         // [오차 제거의 핵심] 시간이 흐른 직후, 현재 READY 상태인 모든 놈들의 대기 시간 증가
//         // 이번 틱에 일을 안 하고 큐에 있던 놈들은 여기서 정확히 1틱씩 보상받습니다.
//         // -------------------------------------------------------
//         for (int i = 0; i < ptable->count; i++) {
//             if (ptable->entries[i].state == PROCESS_READY) {
//                 ptable->entries[i].waiting_time++;
//             }
//         }

//         // 4. 실행 결과에 따른 상태 전이 (다음 틱 준비)
//         if (p->remaining_time <= 0) { // 종료
//             p->state = PROCESS_TERMINATED;
//             p->end_time = global_time;
//             scheduler->running_proc_idx = -1;
//         } 
//         else if (p->io_enabled && p->executed_time == p->io_at) { // I/O 발생
//             p->state = PROCESS_WAITING;
//             p->io_remaining = p->io_wait;
//             queue_push(&scheduler->waiting_queue, scheduler->running_proc_idx);
//             scheduler->running_proc_idx = -1;
//         } 
//         else if (scheduler->algorithm == SCHEDULER_RR && scheduler->current_quantum >= scheduler->rr_quantum) { // 퀀텀 만료
//             p->state = PROCESS_READY;
//             scheduler_enqueue_ready(scheduler, scheduler->running_proc_idx);
//             scheduler->running_proc_idx = -1;
//         }
//     } else {
//         global_time++; // 아무도 없을 때
//     }

//     visualize_system_state(scheduler, ptable);
// }

// void scheduler_run(Scheduler *scheduler, void *ptable_ptr) {
//     if (!scheduler_is_configured(scheduler)) return;
//     while (scheduler_ready_count(scheduler) > 0 || 
//            queue_size(&scheduler->waiting_queue) > 0 || 
//            scheduler->running_proc_idx != -1) {
//         scheduler_step(scheduler, ptable_ptr);
//     }
// }

// void scheduler_reset(Scheduler *scheduler) {
//     scheduler_init(scheduler);
//     global_time = 0;
// }

// // void scheduler_print_stats(const Scheduler *scheduler) {
// //     printf("Current Algorithm: %s\n", scheduler_algorithm_name(scheduler->algorithm));
// //     printf("System Global Time: %d\n", global_time);
// // }

// void scheduler_print_stats(const Scheduler *scheduler, void *ptable_ptr) {
//     ProcessTable *ptable = (ProcessTable *)ptable_ptr;
//     printf("Scheduling Algorithm: %s\n", scheduler_algorithm_name(scheduler->algorithm));
//     if (scheduler->algorithm == SCHEDULER_RR) printf("Quantum: %d\n", scheduler->rr_quantum);

//     for (int i = 0; i < ptable->count; i++) {
//         PCB *p = &ptable->entries[i];
//         printf("%s  Start=%d  End=%d  Waiting=%d\n", 
//                p->name, p->start_time, p->end_time, p->waiting_time);
//     }

//     printf("Context Switch Count: %d\n", scheduler->context_switches);
//     printf("Total Completion Time: %d\n", global_time);
// }




#include <stdio.h>
#include "core/scheduler/scheduler.h"
#include "core/process/process.h" 

// 전역 시스템 시간
static int global_time = 0; 

// [수정] 시스템 현재 상태 시각화 출력 (이름 출력 버전)
static void visualize_system_state(Scheduler *scheduler, ProcessTable *ptable) {
    printf("\nAlgorithm: %s\n", scheduler_algorithm_name(scheduler->algorithm));
    if (scheduler->algorithm == SCHEDULER_RR) {
        printf("Quantum: %d\n", scheduler->rr_quantum);
    }
    
    // 현재 실행 중인 프로세스 이름 출력
    if (scheduler->running_proc_idx != -1) {
        printf("Running: %s\n", ptable->entries[scheduler->running_proc_idx].name);
    } else {
        printf("Running: None\n");
    }

    // [수정] Ready Queue에 있는 프로세스 이름들을 직접 출력
    printf("Ready Queue: ");
    int found_ready = 0;
    for (int i = 0; i < ptable->count; i++) {
        if (ptable->entries[i].state == PROCESS_READY) {
            printf("%s ", ptable->entries[i].name);
            found_ready = 1;
        }
    }
    if (!found_ready) printf("Empty");
    printf("\n");

    // [수정] Waiting Queue에 있는 프로세스 이름들을 직접 출력
    printf("Waiting Queue: ");
    int found_waiting = 0;
    for (int i = 0; i < ptable->count; i++) {
        if (ptable->entries[i].state == PROCESS_WAITING) {
            printf("%s ", ptable->entries[i].name);
            found_waiting = 1;
        }
    }
    if (!found_waiting) printf("Empty");
    printf("\n");

    // 종료된 프로세스 명단 출력
    printf("Terminated: ");
    int found_term = 0;
    for (int i = 0; i < ptable->count; i++) {
        if (ptable->entries[i].state == PROCESS_TERMINATED) {
            printf("%s ", ptable->entries[i].name);
            found_term = 1;
        }
    }
    if (!found_term) printf("Empty");

    printf("\nContext Switches: %d\n", scheduler->context_switches);
    printf("Global Time: %d\n\n", global_time);
}

void scheduler_init(Scheduler *scheduler) {
    scheduler->algorithm = SCHEDULER_FCFS;
    scheduler->is_configured = 0;
    scheduler->rr_quantum = DEFAULT_RR_QUANTUM;
    scheduler->running_proc_idx = -1;
    scheduler->current_quantum = 0;
    scheduler->context_switches = 0;
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

void scheduler_set_algorithm(Scheduler *scheduler, SchedulerAlgorithm algo, int quantum) {
    scheduler->algorithm = algo;
    scheduler->rr_quantum = quantum;
    scheduler->is_configured = 1;
    printf("Scheduling algorithm set to %s.\n", scheduler_algorithm_name(algo));
}

void scheduler_step(Scheduler *scheduler, void *ptable_ptr) {
    ProcessTable *ptable = (ProcessTable *)ptable_ptr;

    // 1. Waiting Queue 처리 (I/O)
    int waiting_count = queue_size(&scheduler->waiting_queue);
    for (int i = 0; i < waiting_count; i++) {
        int w_idx;
        queue_pop(&scheduler->waiting_queue, &w_idx);
        PCB *wp = &ptable->entries[w_idx];
        wp->io_remaining--;
        if (wp->io_remaining <= 0) {
            wp->state = PROCESS_READY;
            scheduler_enqueue_ready(scheduler, w_idx);
            printf("Time %d: [I/O Completed] %s back to Ready Queue\n", global_time, wp->name);
        } else {
            queue_push(&scheduler->waiting_queue, w_idx);
        }
    }

    // 2. CPU 할당 (비어있으면 즉시 할당)
    if (scheduler->running_proc_idx == -1) {
        if (queue_pop(&scheduler->ready_queue, &scheduler->running_proc_idx) == 0) {
            PCB *p = &ptable->entries[scheduler->running_proc_idx];
            p->state = PROCESS_RUNNING;
            if (p->executed_time == 0) p->start_time = global_time;
            scheduler->current_quantum = 0;
            scheduler->context_switches++;
        }
    }

    // 3. 프로세스 실행 및 시간 증가
    if (scheduler->running_proc_idx != -1) {
        PCB *p = &ptable->entries[scheduler->running_proc_idx];
        p->executed_time++;
        p->remaining_time--;
        global_time++; 
        scheduler->current_quantum++;

        // [대기 시간 업데이트] 현재 READY 상태인 모든 프로세스의 대기 시간 증가
        for (int i = 0; i < ptable->count; i++) {
            if (ptable->entries[i].state == PROCESS_READY) {
                ptable->entries[i].waiting_time++;
            }
        }

        // 4. 상태 체크 및 다음 틱 준비
        if (p->remaining_time <= 0) { // 종료
            p->state = PROCESS_TERMINATED;
            p->end_time = global_time;
            scheduler->running_proc_idx = -1;
        } 
        else if (p->io_enabled && p->executed_time == p->io_at) { // I/O 발생
            p->state = PROCESS_WAITING;
            p->io_remaining = p->io_wait;
            queue_push(&scheduler->waiting_queue, scheduler->running_proc_idx);
            scheduler->running_proc_idx = -1;
        } 
        else if (scheduler->algorithm == SCHEDULER_RR && scheduler->current_quantum >= scheduler->rr_quantum) { // 퀀텀 만료
            p->state = PROCESS_READY;
            scheduler_enqueue_ready(scheduler, scheduler->running_proc_idx);
            scheduler->running_proc_idx = -1;
        }
    } else {
        global_time++; 
    }

    // 최종 시각화 출력 (이름 표시 버전 호출)
    visualize_system_state(scheduler, ptable);
}

void scheduler_run(Scheduler *scheduler, void *ptable_ptr) {
    if (!scheduler_is_configured(scheduler)) return;
    while (scheduler_ready_count(scheduler) > 0 || 
           queue_size(&scheduler->waiting_queue) > 0 || 
           scheduler->running_proc_idx != -1) {
        scheduler_step(scheduler, ptable_ptr);
    }
}

void scheduler_reset(Scheduler *scheduler) {
    scheduler_init(scheduler);
    global_time = 0;
    printf("Scheduler has been reset. Global time is now 0.\n");
}

void scheduler_print_stats(const Scheduler *scheduler, void *ptable_ptr) {
    ProcessTable *ptable = (ProcessTable *)ptable_ptr;
    printf("\nScheduling Algorithm: %s\n", scheduler_algorithm_name(scheduler->algorithm));
    if (scheduler->algorithm == SCHEDULER_RR) printf("Quantum: %d\n", scheduler->rr_quantum);

    for (int i = 0; i < ptable->count; i++) {
        PCB *p = &ptable->entries[i];
        if (p->state == PROCESS_TERMINATED) {
            printf("%-5s  Start=%-2d  End=%-2d  Waiting=%-2d\n", 
                   p->name, p->start_time, p->end_time, p->waiting_time);
        }
    }

    printf("Context Switch Count: %d\n", scheduler->context_switches);
    printf("Total Completion Time: %d\n", global_time);
}