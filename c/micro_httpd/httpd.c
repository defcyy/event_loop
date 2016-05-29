#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <strings.h>
#include <error.h>
#include <pthread.h>
#include <regex.h>
#include "httpd.h"


void error_exit(char *error_msg) {
    perror(error_msg);
    exit(EXIT_FAILURE);
}

int server_setup(unsigned int addr, unsigned short port) {
    int socket_fd;
    struct sockaddr_in server_addr;
    int backlog = 5;
    int reuse = 1;

    if ((socket_fd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
        error_exit("Error socket init ");
    }
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse))  == -1) {
        error_exit("Error setsockopt.");
    }

    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(addr);

    if (bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        error_exit("Error on bind.");
    }
    DEBUG_PRINT("Server on %d ...\n", port);
    if (listen(socket_fd, backlog) == -1) {
        error_exit("Error on listen");
    }

    return socket_fd;
}

int read_util_regex(int fd, const char *preg, char *buffer, int max_size) {
    int buffer_size = 1024, ret;
    char buffer_item[buffer_size];
    regex_t regex;
    regmatch_t regmath[1];

    ret = regcomp(&regex, preg, REG_EXTENDED);
    if (ret) {
        DEBUG_PRINT("read regex compile ret -> %d\n", ret);
        return -1;
    }

    int read_size = 0, total_read = 0;
    for (;;) {
        read_size = read(fd, buffer_item, buffer_size);
        if (read_size < 0) {
            break;
        }
        read_size = read_size + total_read >= max_size ? max_size - total_read : read_size;
        DEBUG_PRINT("read -> %d\n", read_size);
        ret = regexec(&regex, buffer_item, 1, regmath, 0);
        if (ret == 0) {
            read_size = read_size > regmath[0].rm_so ? regmath[0].rm_so : read_size;
        }
        strncpy(buffer + total_read, buffer_item, read_size);
        total_read += read_size;

        if (total_read >= max_size) {
            break;
        }
    }

    regfree(&regex);
    return total_read;
}

void accept_request(void *connect) {
    int connect_sock = *(int *)connect;
    pthread_t tid = pthread_self();
    DEBUG_PRINT("Thread id %d serve for socket %d.\n", (long)tid, connect_sock);

    int fflag = fcntl(connect_sock, F_GETFL, 0);
    fcntl(connect_sock, F_SETFL, fflag | O_NONBLOCK);

    char header_buffer[MAX_HEADER_SIZE];
    read_util_regex(connect_sock, "\r\n\r\n", header_buffer, 5);

    fprintf(stdout, "header:\n");
    fprintf(stdout, header_buffer);

    fflush(stdout);
    close(connect_sock);
}


int main(int argc, char *argv[]) {
    int server_sock, connect_sock;
    struct sockaddr_in  client_addr;
    socklen_t client_addr_len;

    pthread_t conn_thread;

    unsigned int listen_addr = INADDR_ANY;
    unsigned short listen_port = 9999;

    client_addr_len = sizeof(client_addr);
    server_sock = server_setup(listen_addr, listen_port);

    for (;;) {
        connect_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_len);
        if (connect_sock == -1) {
            perror("Accept error.\n");
            continue;
        }

        if (pthread_create(&conn_thread, NULL, accept_request, (void *)&connect_sock) != 0) {
            perror("Create accept thread failed.\n");
        }
    }

    close(server_sock);

}
