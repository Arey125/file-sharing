#include <sqlite3.h>

int init_db();

int get_chunk_from_db(char* id, char** chunk);
