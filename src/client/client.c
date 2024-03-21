#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

char* read_response(int client_fd) {
    char length_str[32];
    bzero(length_str, 32);
    read(client_fd, length_str, 32);
    int length = strtol(length_str, NULL, 10);

    char *buf = malloc(length);
    bzero(buf, length);
    read(client_fd, buf, length);
    return buf;
}

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

int get_file_list() {
    int client_fd = setup_socket();
    if (client_fd < 0) {
        return -1;
    }

    write(client_fd, "/", 1);

    char *response = read_response(client_fd);
    printf("File list:\n%s\n", response);
    free(response);

    return 0;
}

int get_chunk(char *chunk_name) {
    int client_fd = setup_socket();
    if (client_fd < 0) {
        return -1;
    }

    write(client_fd, "/chunk ", 7);
    write(client_fd, chunk_name, strlen(chunk_name)); 

    char *chunk = read_response(client_fd);
    printf("Chunk:\n%s\n", chunk);
    free(chunk);

    return 0;
}

int client(int argc, char *argv[]) {
    if (argc == 2 && strcmp("files", argv[1]) == 0) {
        return get_file_list();
    }

    if (argc == 3 && strcmp("chunk", argv[1]) == 0) {
        return get_chunk(argv[2]);
    }
    return 0;
}
