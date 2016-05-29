#include <stdio.h>

#define DEBUG 1
#define MAX_HEADER_SIZE (1<<12)
#define MAX_BODY_SIZE (1<<20)

#define DEBUG_PRINT(fmt, ...) \
    do { if (DEBUG) fprintf(stderr, fmt, __VA_ARGS__); } while (0)


void error_exit(char *);
void accept_request(void *);

int read_util_regex(int fd, const char *preg, char *buffer, int max_size);

int server_setup(unsigned int addr, unsigned short port);
