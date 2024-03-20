#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

void get_file_list_request(int sockfd) {
    char response[] = "1.txt\n2.txt\n3.txt\n";
    write(sockfd, response, strlen(response));
}

void get_chunk_request(int sockfd, char *chunk_name) {
    char response[] = "1.txt";
    write(sockfd, response, strlen(response));
}

void route_request(int sockfd, char *request) {
    char path[1024];
    sscanf(request, "%s", path);

    if (strcmp(path, "/") == 0) {
        get_file_list_request(sockfd);
        return;
    }
    if (strncmp(path, "/chunk", 7) == 0) {
        char chunk_name[1024];
        sscanf(request, "/chunk %s", chunk_name);
        get_chunk_request(sockfd, chunk_name);
        return;
    }
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        printf("To start webserver run: %s", argv[0]);
        return 1;
    }
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return 1;
    }

    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        return 1;
    }
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9115);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sockfd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        perror("bind");
        return 1;
    }

    if (listen(sockfd, 10) < 0) {
        perror("listen");
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
        route_request(newsockfd, buf);
        close(newsockfd);
    }

    return 0;
}
