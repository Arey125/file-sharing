#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "db.h"
#include "router.h"

int write_response_with_len(int sockfd, char *response, int len) {
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

int write_response(int sockfd, char *response) {
    int len = strlen(response);
    return write_response_with_len(sockfd, response, len);
}

int get_file_list_request(int sockfd) {
    char *response = get_file_list_from_db();
    int rc = write_response(sockfd, response);
    free(response);
    return rc;
}

int get_chunk_request(int sockfd, char *chunk_name) {
    char *content;
    int len = get_chunk_from_db(chunk_name, &content);

    int rc = write_response_with_len(sockfd, content, len);
    free(content);
    return rc;
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
