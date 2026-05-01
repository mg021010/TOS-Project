#include "core/memory/memory.h"
#include <string.h>

void memory_init(MemoryManager *mm) {
    memset(mm->ram, 0, PHYSICAL_MEMORY_SIZE);
    for (int i = 0; i < 16; i++) {
        mm->page_table[i].frame_number = -1;
        mm->page_table[i].is_valid = 0;
    }
    for (int i = 0; i < NUM_FRAMES; i++) {
        mm->frame_occupied[i] = 0;
    }
}

// 빈 프레임을 찾아 페이지를 할당
int memory_allocate(MemoryManager *mm, int page_number) {
    if (page_number < 0 || page_number >= 16) return -1;
    if (mm->page_table[page_number].is_valid) return 0; // 이미 할당됨

    for (int i = 0; i < NUM_FRAMES; i++) {
        if (!mm->frame_occupied[i]) {
            mm->frame_occupied[i] = 1;
            mm->page_table[page_number].frame_number = i;
            mm->page_table[page_number].is_valid = 1;
            return i; // 할당된 프레임 번호 반환
        }
    }
    return -2; // 메모리 부족
}

// 가상 주소를 물리 주소로 변환 (Address Translation)
int memory_translate(MemoryManager *mm, int virtual_addr, int *physical_addr) {
    int page_number = virtual_addr / PAGE_SIZE;
    int offset = virtual_addr % PAGE_SIZE;

    if (page_number < 0 || page_number >= 16) return -1; // 잘못된 가상 주소
    if (!mm->page_table[page_number].is_valid) return -2; // 페이지 폴트 (Page Fault)

    int frame_number = mm->page_table[page_number].frame_number;
    *physical_addr = (frame_number * PAGE_SIZE) + offset;
    return 0;
}

void memory_dump(MemoryManager *mm) {
    printf("\n--- Page Table ---\n");
    for (int i = 0; i < 16; i++) {
        if (mm->page_table[i].is_valid) {
            printf("Page [%02d] -> Frame [%d]\n", i, mm->page_table[i].frame_number);
        }
    }
    printf("------------------\n");
}