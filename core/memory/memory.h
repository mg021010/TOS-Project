#ifndef CORE_MEMORY_MEMORY_H
#define CORE_MEMORY_MEMORY_H

#include <stdint.h>
#include <stdio.h>

#define PHYSICAL_MEMORY_SIZE 1024  // 물리 메모리 크기 (1KB)
#define PAGE_SIZE 128              // 페이지 크기 (128B)
#define NUM_FRAMES (PHYSICAL_MEMORY_SIZE / PAGE_SIZE) // 프레임 개수 (8개)

typedef struct {
    int frame_number;  // 매핑된 프레임 번호 (-1이면 할당 안됨)
    int is_valid;      // 유효 비트
} PageTableEntry;

typedef struct {
    uint8_t ram[PHYSICAL_MEMORY_SIZE]; // 실제 물리 메모리 공간
    PageTableEntry page_table[16];     // 가상 페이지 테이블 (최대 16페이지)
    int frame_occupied[NUM_FRAMES];    // 프레임 사용 여부 체크
} MemoryManager;

void memory_init(MemoryManager *mm);
int memory_allocate(MemoryManager *mm, int page_number);
int memory_translate(MemoryManager *mm, int virtual_addr, int *physical_addr);
void memory_dump(MemoryManager *mm);

#endif