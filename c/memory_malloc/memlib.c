#include <stdio.h>
#include <stdlib.h>
#include "memlib.h"


void mem_init(void) {
    mem_heap = (char *)malloc(MAX_HEAP);
    if (!mem_heap) {
        fprintf(stderr, "ERROR: heap init error.\n");
        exit(EXIT_FAILURE);
    }
    mem_brk = mem_heap;
    mem_max_addr = (char *)(mem_heap + MAX_HEAP);
    fprintf(stdout, "mem_init -> heap: %p, brk: %p, max: %p\n", mem_heap, mem_brk, mem_max_addr);
}

void *mem_sbrk(int increment) {
    char *old_brk = mem_brk;
    if (increment < 0 || (mem_brk + increment) > mem_max_addr) {
        fprintf(stderr, "ERROR: ran out of memory.");
        return NULL;
    }
    mem_brk += increment;
    fprintf(stdout, "mem_sbrk -> heap: %p, brk: %p, max: %p\n", mem_heap, mem_brk, mem_max_addr);
    return (void *)old_brk;
}


