
#define MAX_HEAP (1<<22)

static char *mem_heap;
static char *mem_brk;
static char *mem_max_addr;

void mem_init(void);
void *mem_sbrk(int);
