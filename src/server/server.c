#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <signal.h>
#include <errno.h>
#include <sys/wait.h>

#include <sqlite3.h>


#include "server.h"
#include "router.h"
#include "db.h"

int start_server(char *port) {
    init_db();

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

void sigchld_handler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}

int run_server() {
    int sockfd = start_server("9115");

    struct sigaction sa;
    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    if (sockfd < 0) {
        return 1;
    }

    while (1) {
        int newsockfd = accept(sockfd, NULL, NULL);
        if (newsockfd < 0) {
            perror("accept");
            return 1;
        }
        if (!fork()) {
            char buf[1024];
            bzero(buf, 1024);
            read(newsockfd, buf, 1024);
            printf("%s\n", buf);
            int ret = route_request(newsockfd, buf);
            if (ret < 0) {
                printf("route_request failed\n");
            }
            close(newsockfd);
            exit(0);
        }
        close(newsockfd);
    }

    return 0;
}
