#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <strings.h>
#include <error.h>
#include <pthread.h>
#include <regex.h>
#include <stdbool.h>
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

int request_parser(int fd, char *header, size_t header_size, char *body, size_t body_size) {
    size_t read_size, body_read_size = 0;
    int ret;
    char body_buffer[body_size];
    regex_t regex;
    regmatch_t regmatch[1];

    read_size = read(fd, header, header_size);
    if (read_size < 0) {
        return -1;
    }

    ret = regcomp(&regex, "\r\n\r\n", REG_EXTENDED);
    if (ret) {
        DEBUG_PRINT("read regex compile ret -> %d\n", ret);
        return -1;
    }

    ret = regexec(&regex, header, 1, regmatch, 0);
    if (ret == 0) {
        // found header end flag \r\n\r\n in header string
        body_read_size = read_size - regmatch[0].rm_eo;
        strncpy(body, header[regmatch[0].rm_eo], body_read_size);
        header[regmatch[0].rm_so] = '\0';
        if (read_size == header_size) {
            read(fd, body[body_read_size + 1], body_size - body_read_size - 1);
        }
    }
    else {
        read_size = read(fd, body_buffer, body_size);
        if (read_size < 0) {
            return -1;
        }
        ret = regexec(&regex, body_buffer, 1, regmatch, 0);
        if (ret != 0) {
            // not end of heder yet
            return -1;
        }
        body_read_size = read_size - regmatch[0].rm_eo;
        strncpy(body, body_buffer[regmatch[0].rm_eo], body_read_size);
        if (read_size == body_size) {
            read(fd, body[body_read_size + 1], body_size - body_read_size - 1);
        }
    }

    regfree(&regex);
    return 0;
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

int header_parser(const char *header, http_header headers) {
    char prev_ch, ch;
    char line[MAX_HEADER_SIZE];
    bool first = true;

    int i=0;
    int line_pos = 0;
    for(;;) {
        if (i == 0) {
            prev_ch = ch = header[i];
        }else {
            prev_ch = header[i-1];
            ch = header[i];
        }

        if ((prev_ch == '\r' && ch == '\n') || ch == '\0') {
            line[line_pos] = '\0';
            // line parser
            DEBUG_PRINT("get line -> %s\n", line);
            header_line_parser(line, &headers, first);
            line_pos = 0;
            first = false;
        }
        else {
            line[line_pos] = header[i];
            line_pos++;
        }

        if (ch == '\0') {
            break;
        }
        i++;
    }


}

void header_line_parser(const char *line, http_header *headers, bool first) {
    char buffer[1024];
    int segment = 0;
    int i = 0;
    int pos = 0;
    if (first) {
        for(;;) {
            if (line[i] == ' ' || line[i] == '\0') {
                buffer[pos] = '\0';
                DEBUG_PRINT("line parser -> %s\n", buffer);
                switch(segment) {
                    case 0:
                        strcpy(headers->method, buffer);
                        break;
                    case 1:
                        strcpy(headers->path, buffer);
                        break;
                    case 2:
                        strcpy(headers->protocol, buffer);
                        break;
                }
                pos = 0;
                segment++;
            }
            else {
                buffer[pos] = line[i];
                pos++;
            }

            if (line[i] == '\0') {
                break;
            }
            i++;
        }

        DEBUG_PRINT("method: %s, path: %s, protocol: %s\n", headers->method, headers->path, headers->protocol);
    }
    else {
        char key[1024];
        for(;;) {
            if (segment == 0 && line[i] == ':') {
                buffer[pos] = '\0';
                strcpy(key, buffer);

                // skip space
                pos = 0;
                segment = 1;
                i++;
            }
            else {
                buffer[pos] = line[i];
                pos++;
            }

            if (line[i] == '\0') {
                break;
            }
            i++;
        }

        if (strcmp(key, "Host") == 0) {
            DEBUG_PRINT("host -> %s\n", buffer);
            strcpy(headers->host, buffer);
        }
        else if (strcmp(key, "User-Agent") == 0) {
            DEBUG_PRINT("user_agent: %s\n", buffer);
            strcpy(headers->user_agent, buffer);
        }
        else if (strcmp(key, "Accept") == 0) {
            DEBUG_PRINT("accept: %s\n", buffer);
            strcpy(headers->accept, buffer);
        }
    }
}


void accept_request(void *connect) {
    int connect_sock = *(int *)connect;
    int ret;
    pthread_t tid = pthread_self();
    DEBUG_PRINT("Thread id %d serve for socket %d.\n", (long)tid, connect_sock);

    int fflag = fcntl(connect_sock, F_GETFL, 0);
    fcntl(connect_sock, F_SETFL, fflag | O_NONBLOCK);

    char header[MAX_HEADER_SIZE];
    char body[MAX_BODY_SIZE];
    ret = request_parser(connect_sock, header, MAX_HEADER_SIZE, body, MAX_BODY_SIZE);
    if (ret != 0) {
        DEBUG_PRINT("return ret %d from request_parser.", ret);
        return;
    }

    fprintf(stdout, "header: -> \n");
    fprintf(stdout, header);
    fprintf(stdout, "\n");
    fprintf(stdout, "body: -> \n");
    fprintf(stdout, body);
    fprintf(stdout, "\n");

    http_header headers;
    header_parser(header, headers);

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
