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

    if (start_adding_file_to_db(file_name) < 0) {
        return -1;
    }

    SHA256_CTX file_ctx;
    sha256_init(&file_ctx);
    char chunk_buffer[MAX_CHUNK_SIZE];
    int ind = 0;
    while (!feof(file)) {
        int count = fread(chunk_buffer, 1, MAX_CHUNK_SIZE, file);

        BYTE hash[SHA256_BLOCK_SIZE];
        char hash_str[SHA256_BLOCK_SIZE * 2 + 1];
        SHA256_CTX ctx;
        sha256_init(&ctx);
        sha256_update(&ctx,(BYTE *)chunk_buffer, count);
        sha256_update(&file_ctx,(BYTE *)chunk_buffer, count);
        sha256_final(&ctx, hash);

        convert_to_hex(hash, hash_str);

        if (add_chunk_to_db(hash_str, chunk_buffer, count, ind) < 0) {
            return -1;
        }
        ind++;
    }

    BYTE file_hash[SHA256_BLOCK_SIZE];
    char file_hash_str[SHA256_BLOCK_SIZE * 2 + 1];
    sha256_final(&file_ctx, file_hash);

    convert_to_hex(file_hash, file_hash_str);


    finish_adding_file_to_db(file_hash_str);
    fclose(file);

    return 0;
}
