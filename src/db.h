#include <sqlite3.h>

int init_db();

int get_chunk_from_db(char* id, char** chunk);
char* get_file_list_from_db();

int start_adding_file_to_db(char* id, char* name);
int add_chunk_to_db(char* id, char* content, int content_len, int ind, char* file_id);
int commit_transaction();
