#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int get_host(char *host, struct addrinfo **server_addr) {
    char hostname[256] = {0};
    char port[6] = {0};

    if (sscanf(host, "%[^:]:%s", hostname, port) != 2) {
        perror("sscanf error");
        return -1;
    }

    struct addrinfo hints;
    bzero(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int ret = getaddrinfo(hostname, port, &hints, server_addr);
    if (ret < 0) {
        perror("getaddrinfo error");
        return -1;
    }

    return 0;
}

int setup_socket(struct addrinfo *server_addr) {
    int client_fd = socket(server_addr->ai_family, server_addr->ai_socktype,
                           server_addr->ai_protocol);
    if (client_fd < 0) {
        perror("socket error");
        return -1;
    }

    int ret = connect(client_fd, server_addr->ai_addr, server_addr->ai_addrlen);
    if (ret < 0) {
        perror("connect error");
        return -1;
    }

    return client_fd;
}

char *make_request(int client_fd, char *request) {
    int pos = 0;
    int length = strlen(request);
    while (pos < length) {
        int count = send(client_fd, request + pos, length - pos, 0);
        if (count < 0) {
            perror("send error");
            return NULL;
        }
        pos += count;
    }
    shutdown(client_fd, SHUT_WR);

    char length_str[33];
    bzero(length_str, 33);
    int count = read(client_fd, length_str, 32);
    if (count < 0) {
        perror("read error");
        return NULL;
    }

    length = strtol(length_str, NULL, 10);

    int new_line_pos = 0;
    while (length_str[new_line_pos] != '\n')
        new_line_pos++;

    char *buf = malloc(length + 1);
    bzero(buf, length + 1);
    strcpy(buf, length_str + new_line_pos + 1);

    pos = count - new_line_pos - 1;
    while (pos < length) {
        count = recv(client_fd, buf + pos, length - pos, 0);
        if (count < 0) {
            perror("read error");
            free(buf);
            return NULL;
        }
        pos += count;
    }

    return buf;
}

char *request(char *host, char *body) {
    struct addrinfo *server_addr;
    int ret = get_host(host, &server_addr);
    if (ret < 0) {
        return NULL;
    }

    int client_fd = -1;
    for (struct addrinfo *p = server_addr; p != NULL; p = p->ai_next) {
        char addr_str[256];
        inet_ntop(p->ai_family, ((struct sockaddr_in *)p->ai_addr), addr_str, 256);
        client_fd = setup_socket(p);
        if (client_fd > 0) {
            break;
        }
    }

    freeaddrinfo(server_addr);

    if (client_fd < 0) {
        puts("setup_socket error");
        return NULL;
    }


    return make_request(client_fd, body);
}
