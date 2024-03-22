#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int setup_socket() {
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) {
        perror("socket error");
        return -1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(9115);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    int ret = connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (ret < 0) {
        perror("connect error");
        return -1;
    }

    return client_fd;
}

char *make_request(int client_fd, char *request) {
    write(client_fd, request, strlen(request));
    shutdown(client_fd, SHUT_WR);

    char length_str[32];
    bzero(length_str, 32);
    int count = read(client_fd, length_str, 32);
    if (count < 0) {
        perror("read error");
        return NULL;
    }

    int length = strtol(length_str, NULL, 10);
    printf("Length: %d\n", length);

    int pos = 0;
    while (length_str[pos] != '\n')
        pos++;

    char *buf = malloc(length);
    bzero(buf, length);
    strcpy(buf, length_str + pos + 1);

    if (read(client_fd, buf + count - pos - 1, length - count + pos + 1) < 0) {
        perror("read error");
        free(buf);
        return NULL;
    }
    return buf;
}


int get_file_list() {
    int client_fd = setup_socket();
    if (client_fd < 0) {
        return -1;
    }

    char *response = make_request(client_fd, "/");
    if (response == NULL) {
        return -1;
    }

    printf("File list:\n%s\n", response);
    free(response);

    return 0;
}

int get_chunk(char *chunk_name) {
    int client_fd = setup_socket();
    if (client_fd < 0) {
        return -1;
    }

    char request[1024];
    bzero(request, 1024);
    sprintf(request, "/chunk %s", chunk_name);

    char *chunk = make_request(client_fd, request);
    if (chunk == NULL) {
        return -1;
    }
    printf("Chunk:\n%s\n", chunk);
    free(chunk);

    return 0;
}

int client(int argc, char *argv[]) {
    if (argc == 2 && strcmp("list", argv[1]) == 0) {
        return get_file_list();
    }

    if (argc == 3 && strcmp("chunk", argv[1]) == 0) {
        return get_chunk(argv[2]);
    }
    return 0;
}
