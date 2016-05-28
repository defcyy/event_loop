#include <stdio.h>
#include <stdlib.h>
#include "memlib.h"
#include "mm.h"


static void *coalesce(void *bp) {
    size_t prev_alloc = GET_ALLOC(HDRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));

    if (prev_alloc && next_alloc) {
        return bp;
    }
    else if (prev_alloc && !next_alloc) {
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));
        fprintf(stdout, "coalecse next, new size -> %d\n", size);
    }
    else if (!prev_alloc && next_alloc) {
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUT(FTRP(bp), PACK(size, 0));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        fprintf(stdout, "coalesce prev, new size -> %d\n", size);
    }
    else {
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
        fprintf(stdout, "coalesce both, new size -> %d\n", size);
    }
    return bp;
}

static void *extend_heap(size_t words) {
    char *bp;
    size_t size;

    size = words % 2 ? (words + 1) * WSIZE : words * WSIZE;
    bp = mem_sbrk(size);
    if (!bp) {
        return NULL;
    }

    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));

    return coalesce(bp);
}

static void *find_fit(size_t size) {
    char *bp = heap_listp;
    while (GET_SIZE(HDRP(bp)) > 0 ) {
        // fprintf(stdout, "find_fit: check %p -> size: %d alloc: %d\n", bp, GET_SIZE(HDRP(bp)), GET_ALLOC(HDRP(bp)));
        if (GET_SIZE(HDRP(bp)) >= size && !GET_ALLOC(HDRP(bp))) {
            return bp;
        }
        bp = NEXT_BLKP(bp);
    }
    return NULL;
}

static void place(char *bp, size_t size) {
    if (GET_SIZE(HDRP(bp)) - size < 2*DSIZE) {
        PUT(HDRP(bp), PACK(GET_SIZE(HDRP(bp)), 1));
        PUT(FTRP(bp), PACK(GET_SIZE(HDRP(bp)), 1));
    }else {
        size_t rest_size = GET_SIZE(HDRP(bp)) - size;
        PUT(HDRP(bp), PACK(size, 1));
        PUT(FTRP(bp), PACK(size, 1));
        PUT(HDRP(NEXT_BLKP(bp)), PACK(rest_size, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(rest_size, 0));
    }
}

int mm_init(void) {
    heap_listp = mem_sbrk(4*WSIZE);

    if (!heap_listp) {
        return -1;
    }
    PUT(heap_listp, 0);
    PUT(heap_listp + (WSIZE), PACK(DSIZE, 1));
    PUT(heap_listp + (2*WSIZE), PACK(DSIZE, 1));
    PUT(heap_listp + (3*WSIZE), PACK(0, 1)); // end of heap list

    heap_listp += (2*WSIZE); // increase the mem_brk pointer

    if (!extend_heap(CHUNKSIZE/WSIZE)) {
        return -1;
    }
    return 0;
}

void *mm_malloc(size_t size) {
    size_t alloc_size;
    char *bp;

    if (size == 0) {
        return NULL;
    }
    else if (size <= DSIZE){
        alloc_size = 2 * DSIZE;
    }
    else {
        alloc_size = ((size + DSIZE + (DSIZE-1)) / DSIZE) * DSIZE;
    }

    if ((bp = find_fit(alloc_size)) != NULL) {
        place(bp, alloc_size);
        return bp;
    }
    else {
        while (extend_heap(CHUNKSIZE/WSIZE) != NULL) {
            if ((bp = find_fit(alloc_size)) != NULL) {
                place(bp, alloc_size);
                return bp;
            }
        }
    }
    return NULL;
}

void mm_free(void *bp) {
    PUT(HDRP(bp), PACK(GET_SIZE(HDRP(bp)), 0));
    PUT(FTRP(bp), PACK(GET_SIZE(HDRP(bp)), 0));
    coalesce(bp);
}

void print_heap(void) {
    for (char *bp=heap_listp; GET_SIZE(HDRP(bp)) > 0; bp=NEXT_BLKP(bp)) {
        fprintf(stdout, " -> size: %d, alloc: %d, p: %p\n", GET_SIZE(HDRP(bp)), GET_ALLOC(HDRP(bp)), bp);
    }
    fprintf(stdout, "\n\n");
}
