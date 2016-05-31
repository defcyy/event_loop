#include <stdio.h>

#define DEBUG 1
#define MAX_HEADER_SIZE (1<<12)
#define MAX_BODY_SIZE (1<<20)

#define SERVER_STRING "micro http server"
#define HTML_ROOT "html"

#define DEBUG_PRINT(fmt, ...) \
    do { if (DEBUG) fprintf(stderr, fmt, __VA_ARGS__); } while (0)


// header data struct
typedef struct header_pair {
    char *key;
    char *value;
} header_pair;

typedef struct http_header {
    char method[1024];
    char path[1024];
    char protocol[1024];
    char version[1024];
    char host[1024];
    char user_agent[1024];
    char accept[1024];
} http_header;


void error_exit(char *);
void accept_request(void *);

int read_util_regex(int fd, const char *preg, char *buffer, int max_size);
int request_parser(int fd, char *header, size_t header_size, char *body, size_t body_size);

int server_setup(unsigned int addr, unsigned short port);
