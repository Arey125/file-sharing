#include <stdio.h>
#include <string.h>

#include "db.h"
#include "hash_sha256.h"

const int MAX_CHUNK_SIZE = 1024 * 1024;

int add_file(char *file_path) {
    FILE* file = fopen(file_path, "rb");
    if (!file) {
        return -1;
    }
    char *file_name = file_path; 
    char* lastSlash = strrchr(file_name, '/');
    if (lastSlash) {
        file_name = lastSlash + 1;
    }

    if (strlen(file_name) == 0) {
        return -1;
    }

    init_db();

    if (start_adding_file_to_db(file_name, file_name) < 0) {
        return -1;
    }

    char chunk_buffer[MAX_CHUNK_SIZE];
    int ind = 0;
    while (!feof(file)) {
        int count = fread(chunk_buffer, MAX_CHUNK_SIZE, 1, file);

        char hash[SHA256_BLOCK_SIZE];
        char hash_str[SHA256_BLOCK_SIZE * 2 + 2];
        SHA256_CTX ctx;
        sha256_init(&ctx);
        sha256_update(&ctx,(BYTE *)chunk_buffer, count);
        sha256_final(&ctx, (BYTE *)hash);

        for (int i = 0; i < SHA256_BLOCK_SIZE; i++) {
            sprintf(hash_str + (i * 2), "%02x", (unsigned char)hash[i]);
        }
        hash_str[SHA256_BLOCK_SIZE * 2] = '\0';

        if (add_chunk_to_db(hash_str, chunk_buffer, count, ind, file_name) < 0) {
            return -1;
        }
        ind++;
    }

    return 0;
}
