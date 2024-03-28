#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "request.h"

char **get_server_list() {
    FILE *fp = fopen("./servers", "r");
    if (fp == NULL) {
        perror("fopen error");
        return NULL;
    }

    int server_list_size = 16;
    char **server_list = malloc(sizeof(char *) * server_list_size);
    char buf[1024];
    int pos = 0;
    while (fscanf(fp, "%s", buf) != EOF) {
        if (pos >= server_list_size - 2) {
            server_list_size *= 2;
            server_list =
                realloc(server_list, server_list_size * sizeof(char *));
        }
        server_list[pos] = strdup(buf);
        pos++;
    }
    server_list[pos] = NULL;

    return server_list;
}

int get_file_list(char *host) {
    char *response = request(host, "/");
    if (response == NULL) {
        return -1;
    }

    printf("File list:\n%s\n", response);
    free(response);

    return 0;
}

int get_chunk(char *host, char *chunk_name) {
    char request_body[1024];
    bzero(request_body, 1024);
    sprintf(request_body, "/chunk %s", chunk_name);

    char *chunk = request(host, request_body);
    if (chunk == NULL) {
        return -1;
    }
    printf("Chunk:\n%s\n", chunk);
    free(chunk);

    return 0;
}

int client(int argc, char *argv[]) {
    int ret = 0;
    if (argc == 2 && strcmp("list", argv[1]) == 0) {
        char **server_list = get_server_list();
        for (int i = 0; server_list[i] != NULL; i++) {
            printf("%s\n", server_list[i]);
            if (get_file_list(server_list[i]) < 0) {
                ret = -1;
                break;
            }
        }

        for (int i = 0; server_list[i] != NULL; i++) {
            free(server_list[i]);
        }
        free(server_list);

        return ret;
    }

    if (argc == 3 && strcmp("chunk", argv[1]) == 0) {
        char **server_list = get_server_list();
        for (int i = 0; server_list[i] != NULL; i++) {
            printf("%s\n", server_list[i]);
            if (get_chunk(server_list[i], server_list[i]) < 0) {
                ret = -1;
                break;
            }
        }

        for (int i = 0; server_list[i] != NULL; i++) {
            free(server_list[i]);
        }
        free(server_list);

        return 0;
    }
    return 0;
}
