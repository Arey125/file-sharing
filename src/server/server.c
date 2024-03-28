#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../server.h"
#include "router.h"

int start_server(char *port) {
    struct addrinfo hints;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    struct addrinfo *result;

    int ret = getaddrinfo(NULL, port, &hints, &result);
    if (ret != 0) {
        perror("getaddrinfo");
        exit(1);
    }

    int sockfd = -1;
    for (struct addrinfo *p = result; p != NULL; p = p->ai_next) {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            perror("socket");
            continue;
        }

        int opt = 1;
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) <
            0) {
            perror("setsockopt");
            continue;
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) < 0) {
            perror("bind");
            continue;
        }

        if (listen(sockfd, 256) < 0) {
            perror("listen");
            continue;
        }
    }
    freeaddrinfo(result);

    return sockfd;
}

int run_server() {
    int sockfd = start_server("9115");

    if (sockfd < 0) {
        return 1;
    }

    while (1) {
        int newsockfd = accept(sockfd, NULL, NULL);
        if (newsockfd < 0) {
            perror("accept");
            return 1;
        }

        char buf[1024];
        bzero(buf, 1024);
        read(newsockfd, buf, 1024);
        printf("%s\n", buf);
        int ret = route_request(newsockfd, buf);
        if (ret < 0) {
            printf("route_request failed\n");
        }
        close(newsockfd);
    }

    return 0;
}
