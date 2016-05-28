#include <stdio.h>
#include "memlib.h"
#include "mm.h"

int main(int argc, char *argv) {
    mem_init();

    int res = mm_init();
    printf("mm_init: %d\n", res);

    void *p, *p1, *p2;

    print_heap();
    p = mm_malloc(sizeof(int));
    if (!p) {
        printf("mm_malloc error.\n");
    }
    printf("mm_malloc p: %p\n", p);

    print_heap();

    p1 = mm_malloc(4*sizeof(int));
    if (!p1) {
        printf("mm_malloc error.\n");
    }
    printf("mm_malloc p: %p\n", p1);

    print_heap();
    mm_free(p);

    print_heap();
    p = mm_malloc(sizeof(int));
    if (!p) {
        printf("mm_malloc error.\n");
    }
    printf("mm_malloc p: %p\n", p);

    print_heap();
    p2 = mm_malloc(sizeof(int));
    if (!p2) {
        printf("mm_malloc error.\n");
    }
    printf("mm_malloc p: %p\n", p2);

    print_heap();
    mm_free(p);
    mm_free(p2);
    mm_free(p1);
    print_heap();

    p = mm_malloc(2048);
    p1 = mm_malloc(20);
    print_heap();
    mm_free(p);
    p = mm_malloc(1024);
    print_heap();
    p2 = mm_malloc(2048);
    print_heap();
    mm_free(p1);
    mm_free(p);
    print_heap();
    p = mm_malloc(8000);
    print_heap();
    mm_free(p2);
    mm_free(p);

    print_heap();
}
