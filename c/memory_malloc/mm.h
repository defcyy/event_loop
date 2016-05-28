
static char *heap_listp;

#define WSIZE 4
#define DSIZE 8
#define CHUNKSIZE (1<<12)

#define PACK(size, alloc) ((size) | (alloc))
#define GET(p) (*(unsigned int*)(p))
#define PUT(p, value) (*(unsigned int*)(p) = (value))
#define GET_SIZE(p) (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)
#define HDRP(bp) ((char *)bp - WSIZE)
#define FTRP(bp) ((char *)bp + GET_SIZE(HDRP(bp)) - DSIZE)
#define NEXT_BLKP(bp) ((char *)bp + GET_SIZE(HDRP(bp)))
#define PREV_BLKP(bp) ((char *)bp - GET_SIZE((char *)bp - DSIZE))


int mm_init(void);
void *mm_malloc(size_t);
void mm_free(void *);
void print_heap(void);

