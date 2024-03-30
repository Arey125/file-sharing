#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "db.h"

sqlite3 *db;

int exec(char *sql) {
    char *err_msg = NULL;
    int rc = sqlite3_exec(db, sql, NULL, NULL, &err_msg);
    if (rc != SQLITE_OK) {
        printf("SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    }
    return rc;
}

int init_db() {
    int rc = sqlite3_open("fsh.db", &db);
    if (rc != SQLITE_OK) {
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }

    if (exec("CREATE TABLE IF NOT EXISTS files (id TEXT PRIMARY KEY, name TEXT)") != SQLITE_OK) {
        return -1;
    }

    if (exec("CREATE TABLE IF NOT EXISTS chunks (id TEXT PRIMARY KEY, content BLOB)") != SQLITE_OK) {
        return -1;
    }

    return 0;
};

int get_chunk_from_db(char* id, char** chunk) {
    *chunk = malloc(1);
    int len = 0;

    char sql[256];
    sprintf(sql, "SELECT content FROM chunks WHERE id = '%s'", id);
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        len = sqlite3_column_bytes(stmt, 0);
        *chunk = realloc(*chunk, len);
        memcpy(*chunk, sqlite3_column_blob(stmt, 0), len);
    }
    else {
        printf("SQL error: %s\n", sqlite3_errmsg(db));

    }
    sqlite3_finalize(stmt);

    return len;
}
