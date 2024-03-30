#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "hash_sha256.h"

int write_response(int sockfd, char *response) {
    int len = strlen(response);
    char len_str[32];
    sprintf(len_str, "%d\n", len);

    if (write(sockfd, len_str, strlen(len_str)) < 0) {
        perror("write error");
        return -1;
    }

    int pos = 0;

    while (pos < len) {
        int count = send(sockfd, response + pos, len - pos, 0);
        if (count < 0) {
            perror("write error");
            return -1;
        }
        printf("Sent %d bytes\n", count);
        pos += count;
    }
    return write(sockfd, response, len);
}

int get_file_list_request(int sockfd) {
    char response[] = "1.txt\n2.txt\n3.txt";
    return write_response(sockfd, response);
}

int get_chunk_request(int sockfd, char *chunk_name) {
    char content[] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.\nUt enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";
    SHA256_CTX ctx;
    sha256_init(&ctx);
    sha256_update(&ctx, (BYTE *)content, strlen(content));

    char hash[SHA256_BLOCK_SIZE];
    char hash_str[SHA256_BLOCK_SIZE * 2 + 1];
    sha256_final(&ctx, (BYTE *)hash);
    for (int i = 0; i < SHA256_BLOCK_SIZE; i++) {
        sprintf(hash_str + i * 2, "%02x", (unsigned char)hash[i]);
    }
    hash_str[SHA256_BLOCK_SIZE * 2] = '\0';

    printf("%s\n", chunk_name);
    if (strcmp(chunk_name, hash_str) != 0) {
        write_response(sockfd, "Not found");
        puts("Not found");
        return 0;
    }

    return write_response(sockfd, content);
}

int route_request(int sockfd, char *request) {
    char path[1024];
    sscanf(request, "%s", path);

    if (strcmp(path, "/") == 0) {
        return get_file_list_request(sockfd);
    }
    if (strncmp(path, "/chunk", 7) == 0) {
        char chunk_name[1024];
        sscanf(request, "/chunk %s", chunk_name);
        return get_chunk_request(sockfd, chunk_name);
        return 0;
    }

    return -1;
}
