// #include <errno.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>

// #include "common/util.h"
// #include "core/kernel/kernel.h"
// #include "core/process/process.h"
// #include "user/shell/builtins.h"
// #include "user/shell/shell.h"
// #include "core/scheduler/scheduler.h"
// #include "core/memory/memory.h"

// typedef int (*BuiltinHandler)(Shell *shell, int argc, char *argv[]);

// typedef struct {
//     const char *name;
//     BuiltinHandler handler;
//     const char *usage;
//     const char *description;
// } BuiltinCommand;

// static int builtin_cd(Shell *shell, int argc, char *argv[]);
// static int builtin_help(Shell *shell, int argc, char *argv[]);
// static int builtin_exit(Shell *shell, int argc, char *argv[]);
// static int builtin_pwd(Shell *shell, int argc, char *argv[]);
// static int builtin_create(Shell *shell, int argc, char *argv[]);
// static int builtin_ps(Shell *shell, int argc, char *argv[]);
// static int builtin_set_sched(Shell *shell, int argc, char *argv[]);
// static int builtin_run(Shell *shell, int argc, char *argv[]);
// static int builtin_step(Shell *shell, int argc, char *argv[]);
// static int builtin_stats(Shell *shell, int argc, char *argv[]);
// static int builtin_reset(Shell *shell, int argc, char *argv[]);
// static int builtin_memsim(Shell *shell, int argc, char *argv[]);

// static const BuiltinCommand builtin_commands[] = {
//     { "cd", builtin_cd, "cd <dir>", "Change directory" },
//     { "help", builtin_help, "help", "Show help" },
//     { "exit", builtin_exit, "exit", "Exit shell" },
//     { "pwd", builtin_pwd, "pwd", "Print directory" },
//     { "create", builtin_create, "create <name> <time> [io <at> <wait>]", "Create process" },
//     { "ps", builtin_ps, "ps", "Process snapshot" },
//     { "set_sched", builtin_set_sched, "set_sched <fcfs|rr> [q]", "Set algorithm" },
//     { "run", builtin_run, "run", "Run all" },
//     { "step", builtin_step, "step", "Run one tick" },
//     { "stats", builtin_stats, "stats", "Show stats" },
//     { "reset", builtin_reset, "reset", "Reset all" },
//     { "memsim", builtin_memsim, "memsim <file>", "Memory simulation" }
// };

// static const size_t builtin_command_count = sizeof(builtin_commands) / sizeof(builtin_commands[0]);

// static int builtin_cd(Shell *shell, int argc, char *argv[]) {
//     if (argc < 2) return BUILTIN_CONTINUE;
//     if (chdir(argv[1]) == -1) perror("cd");
//     return BUILTIN_CONTINUE;
// }

// static int builtin_help(Shell *shell, int argc, char *argv[]) {
//     for (size_t i = 0; i < builtin_command_count; i++) {
//         printf("  %-34s %s\n", builtin_commands[i].usage, builtin_commands[i].description);
//     }
//     return BUILTIN_CONTINUE;
// }

// static int builtin_exit(Shell *shell, int argc, char *argv[]) { return BUILTIN_EXIT; }

// static int builtin_pwd(Shell *shell, int argc, char *argv[]) {
//     char *cwd = util_getcwd_alloc();
//     if (cwd) { printf("%s\n", cwd); free(cwd); }
//     return BUILTIN_CONTINUE;
// }

// static int builtin_create(Shell *shell, int argc, char *argv[]) {
//     if (argc != 3 && argc != 6) { printf("Usage: create <name> <total_time> [io <io_at> <io_wait>]\n"); return BUILTIN_CONTINUE; }
//     int total, io_at = 0, io_wait = 0, io_en = (argc == 6);
//     if (util_parse_positive_int(argv[2], &total) != 0) return BUILTIN_CONTINUE;
//     if (io_en) {
//         if (strcmp(argv[3], "io") != 0 || util_parse_positive_int(argv[4], &io_at) != 0 || util_parse_positive_int(argv[5], &io_wait) != 0) return BUILTIN_CONTINUE;
//     }
//     PCB *pcb;
//     if (kernel_create_process(&shell->kernel, argv[1], total, io_en, io_at, io_wait, &pcb) == 0) {
//         printf("Created %s (PID=%d)\n", pcb->name, pcb->logical_pid);
//     }
//     return BUILTIN_CONTINUE;
// }

// static int builtin_ps(Shell *shell, int argc, char *argv[]) {
//     kernel_print_process_snapshot(&shell->kernel, stdout);
//     return BUILTIN_CONTINUE;
// }

// static int builtin_set_sched(Shell *shell, int argc, char *argv[]) {
//     if (argc < 2) return BUILTIN_CONTINUE;
//     if (strcmp(argv[1], "fcfs") == 0) scheduler_set_algorithm(&shell->kernel.scheduler, SCHEDULER_FCFS, 0);
//     else if (strcmp(argv[1], "rr") == 0) {
//         int q = (argc >= 3) ? atoi(argv[2]) : DEFAULT_RR_QUANTUM;
//         scheduler_set_algorithm(&shell->kernel.scheduler, SCHEDULER_RR, q);
//     }
//     return BUILTIN_CONTINUE;
// }

// static int builtin_run(Shell *shell, int argc, char *argv[]) {
//     scheduler_run(&shell->kernel.scheduler, &shell->kernel.process_table);
//     return BUILTIN_CONTINUE;
// }

// static int builtin_step(Shell *shell, int argc, char *argv[]) {
//     if (!scheduler_is_configured(&shell->kernel.scheduler)) return BUILTIN_CONTINUE;
//     scheduler_step(&shell->kernel.scheduler, &shell->kernel.process_table);
//     return BUILTIN_CONTINUE;
// }

// // static int builtin_stats(Shell *shell, int argc, char *argv[]) {
// //     scheduler_print_stats(&shell->kernel.scheduler);
// //     return BUILTIN_CONTINUE;
// // }
// static int builtin_stats(Shell *shell, int argc, char *argv[]) {
//     (void)argc; (void)argv;
//     // 두 번째 인자로 프로세스 테이블 주소를 전달합니다.
//     scheduler_print_stats(&shell->kernel.scheduler, &shell->kernel.process_table);
//     return BUILTIN_CONTINUE;
// }

// static int builtin_reset(Shell *shell, int argc, char *argv[]) {
//     scheduler_reset(&shell->kernel.scheduler);
//     process_table_init(&shell->kernel.process_table);
//     return BUILTIN_CONTINUE;
// }

// int builtins_execute(Shell *shell, int argc, char *argv[]) {
//     if (argc == 0) return BUILTIN_CONTINUE;
//     for (size_t i = 0; i < builtin_command_count; i++) {
//         if (strcmp(argv[0], builtin_commands[i].name) == 0) return builtin_commands[i].handler(shell, argc, argv);
//     }
//     return BUILTIN_NOT_FOUND;
// }

// static int builtin_memsim(Shell *shell, int argc, char *argv[]) {
//     if (argc < 2) {
//         fprintf(stderr, "Usage: memsim <scenario_file>\n");
//         return BUILTIN_CONTINUE;
//     }

//     FILE *fp = fopen(argv[1], "r");
//     if (!fp) {
//         perror("memsim");
//         return BUILTIN_CONTINUE;
//     }

//     MemoryManager mm;
//     memory_init(&mm);

//     char command[16];
//     int val;

//     printf("Starting Memory Simulation using file: %s\n", argv[1]);

//     while (fscanf(fp, "%s %d", command, &val) != EOF) {
//         if (strcmp(command, "ALLOC") == 0) {
//             int res = memory_allocate(&mm, val);
//             if (res >= 0) printf("[ALLOC] Page %d assigned to Frame %d\n", val, res);
//             else printf("[ALLOC] Failed to allocate Page %d\n", val);
//         } 
//         else if (strcmp(command, "ACCESS") == 0) {
//             int p_addr;
//             int res = memory_translate(&mm, val, &p_addr);
//             if (res == 0) {
//                 printf("[ACCESS] VA %d -> PA %d\n", val, p_addr);
//             } else if (res == -2) {
//                 printf("[ACCESS] VA %d -> Page Fault!\n", val);
//             } else {
//                 printf("[ACCESS] VA %d -> Invalid Address\n", val);
//             }
//         }
//     }

//     memory_dump(&mm);
//     fclose(fp);
//     return BUILTIN_CONTINUE;
// }


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "common/util.h"
#include "core/kernel/kernel.h"
#include "core/process/process.h"
#include "user/shell/builtins.h"
#include "user/shell/shell.h"
#include "core/scheduler/scheduler.h"
#include "core/memory/memory.h" // 메모리 헤더 추가

typedef int (*BuiltinHandler)(Shell *shell, int argc, char *argv[]);

typedef struct {
    const char *name;
    BuiltinHandler handler;
    const char *usage;
    const char *description;
} BuiltinCommand;

// --- [수정] 함수 선언부 (여기에 등록해야 배열에서 에러가 안 납니다) ---
static int builtin_cd(Shell *shell, int argc, char *argv[]);
static int builtin_help(Shell *shell, int argc, char *argv[]);
static int builtin_exit(Shell *shell, int argc, char *argv[]);
static int builtin_pwd(Shell *shell, int argc, char *argv[]);
static int builtin_create(Shell *shell, int argc, char *argv[]);
static int builtin_ps(Shell *shell, int argc, char *argv[]);
static int builtin_set_sched(Shell *shell, int argc, char *argv[]);
static int builtin_run(Shell *shell, int argc, char *argv[]);
static int builtin_step(Shell *shell, int argc, char *argv[]);
static int builtin_stats(Shell *shell, int argc, char *argv[]);
static int builtin_reset(Shell *shell, int argc, char *argv[]);
static int builtin_memsim(Shell *shell, int argc, char *argv[]); // memsim 선언 추가

static const BuiltinCommand builtin_commands[] = {
    { "cd", builtin_cd, "cd <dir>", "Change directory" },
    { "help", builtin_help, "help", "Show help" },
    { "exit", builtin_exit, "exit", "Exit shell" },
    { "pwd", builtin_pwd, "pwd", "Print directory" },
    { "create", builtin_create, "create <name> <time> [io <at> <wait>]", "Create process" },
    { "ps", builtin_ps, "ps", "Process snapshot" },
    { "set_sched", builtin_set_sched, "set_sched <fcfs|rr> [q]", "Set algorithm" },
    { "run", builtin_run, "run", "Run all" },
    { "step", builtin_step, "step", "Run one tick" },
    { "stats", builtin_stats, "stats", "Show stats" },
    { "reset", builtin_reset, "reset", "Reset all" },
    { "memsim", builtin_memsim, "memsim <file>", "Memory simulation" } // memsim 등록
};

static const size_t builtin_command_count = sizeof(builtin_commands) / sizeof(builtin_commands[0]);

// --- [수정] 핸들러 구현부 (경고 제거 포함) ---

static int builtin_cd(Shell *shell, int argc, char *argv[]) {
    (void)shell; // 경고 제거
    if (argc < 2) return BUILTIN_CONTINUE;
    if (chdir(argv[1]) == -1) perror("cd");
    return BUILTIN_CONTINUE;
}

static int builtin_help(Shell *shell, int argc, char *argv[]) {
    (void)shell; (void)argc; (void)argv; // 경고 제거
    for (size_t i = 0; i < builtin_command_count; i++) {
        printf("  %-34s %s\n", builtin_commands[i].usage, builtin_commands[i].description);
    }
    return BUILTIN_CONTINUE;
}

static int builtin_exit(Shell *shell, int argc, char *argv[]) { 
    (void)shell; (void)argc; (void)argv;
    return BUILTIN_EXIT; 
}

static int builtin_pwd(Shell *shell, int argc, char *argv[]) {
    (void)shell; (void)argc; (void)argv;
    char *cwd = util_getcwd_alloc();
    if (cwd) { printf("%s\n", cwd); free(cwd); }
    return BUILTIN_CONTINUE;
}

static int builtin_create(Shell *shell, int argc, char *argv[]) {
    if (argc != 3 && argc != 6) { printf("Usage: create <name> <total_time> [io <io_at> <io_wait>]\n"); return BUILTIN_CONTINUE; }
    int total, io_at = 0, io_wait = 0, io_en = (argc == 6);
    if (util_parse_positive_int(argv[2], &total) != 0) return BUILTIN_CONTINUE;
    if (io_en) {
        if (strcmp(argv[3], "io") != 0 || util_parse_positive_int(argv[4], &io_at) != 0 || util_parse_positive_int(argv[5], &io_wait) != 0) return BUILTIN_CONTINUE;
    }
    PCB *pcb;
    if (kernel_create_process(&shell->kernel, argv[1], total, io_en, io_at, io_wait, &pcb) == 0) {
        printf("Created %s (PID=%d)\n", pcb->name, pcb->logical_pid);
    }
    return BUILTIN_CONTINUE;
}

static int builtin_ps(Shell *shell, int argc, char *argv[]) {
    (void)argc; (void)argv;
    kernel_print_process_snapshot(&shell->kernel, stdout);
    return BUILTIN_CONTINUE;
}

static int builtin_set_sched(Shell *shell, int argc, char *argv[]) {
    if (argc < 2) return BUILTIN_CONTINUE;
    if (strcmp(argv[1], "fcfs") == 0) scheduler_set_algorithm(&shell->kernel.scheduler, SCHEDULER_FCFS, 0);
    else if (strcmp(argv[1], "rr") == 0) {
        int q = (argc >= 3) ? atoi(argv[2]) : DEFAULT_RR_QUANTUM;
        scheduler_set_algorithm(&shell->kernel.scheduler, SCHEDULER_RR, q);
    }
    return BUILTIN_CONTINUE;
}

static int builtin_run(Shell *shell, int argc, char *argv[]) {
    (void)argc; (void)argv;
    scheduler_run(&shell->kernel.scheduler, &shell->kernel.process_table);
    return BUILTIN_CONTINUE;
}

static int builtin_step(Shell *shell, int argc, char *argv[]) {
    (void)argc; (void)argv;
    if (!scheduler_is_configured(&shell->kernel.scheduler)) return BUILTIN_CONTINUE;
    scheduler_step(&shell->kernel.scheduler, &shell->kernel.process_table);
    return BUILTIN_CONTINUE;
}

static int builtin_stats(Shell *shell, int argc, char *argv[]) {
    (void)argc; (void)argv;
    scheduler_print_stats(&shell->kernel.scheduler, &shell->kernel.process_table);
    return BUILTIN_CONTINUE;
}

static int builtin_reset(Shell *shell, int argc, char *argv[]) {
    (void)argc; (void)argv;
    scheduler_reset(&shell->kernel.scheduler);
    process_table_init(&shell->kernel.process_table);
    return BUILTIN_CONTINUE;
}

// --- [추가] 6번 메모리 시뮬레이션 핸들러 ---
static int builtin_memsim(Shell *shell, int argc, char *argv[]) {
    (void)shell;
    if (argc < 2) {
        fprintf(stderr, "Usage: memsim <scenario_file>\n");
        return BUILTIN_CONTINUE;
    }

    FILE *fp = fopen(argv[1], "r");
    if (!fp) {
        perror("memsim");
        return BUILTIN_CONTINUE;
    }

    MemoryManager mm;
    memory_init(&mm);

    char command[16];
    int val;

    printf("Starting Memory Simulation using file: %s\n", argv[1]);

    while (fscanf(fp, "%s %d", command, &val) != EOF) {
        if (strcmp(command, "ALLOC") == 0) {
            int res = memory_allocate(&mm, val);
            if (res >= 0) printf("[ALLOC] Page %d assigned to Frame %d\n", val, res);
            else printf("[ALLOC] Failed to allocate Page %d\n", val);
        } 
        else if (strcmp(command, "ACCESS") == 0) {
            int p_addr;
            int res = memory_translate(&mm, val, &p_addr);
            if (res == 0) {
                printf("[ACCESS] VA %d -> PA %d\n", val, p_addr);
            } else if (res == -2) {
                printf("[ACCESS] VA %d -> Page Fault!\n", val);
            } else {
                printf("[ACCESS] VA %d -> Invalid Address\n", val);
            }
        }
    }

    memory_dump(&mm);
    fclose(fp);
    return BUILTIN_CONTINUE;
}

int builtins_execute(Shell *shell, int argc, char *argv[]) {
    if (argc == 0) return BUILTIN_CONTINUE;
    for (size_t i = 0; i < builtin_command_count; i++) {
        if (strcmp(argv[0], builtin_commands[i].name) == 0) return builtin_commands[i].handler(shell, argc, argv);
    }
    return BUILTIN_NOT_FOUND;
}