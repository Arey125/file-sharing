#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>


void get_file_list_request(int sockfd) {
    char response[] = "1.txt\n2.txt\n3.txt";
    write(sockfd, response, strlen(response));
}

void get_chunk_request(int sockfd, char *chunk_name) {
    char content[] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";
    int len = strlen(content);
    char len_str[32];
    sprintf(len_str, "%d\n", len);

    write(sockfd, len_str, strlen(len_str));
    write(sockfd, content, strlen(content));
}

void route_request(int sockfd, char *request) {
    char path[1024];
    sscanf(request, "%s", path);

    if (strcmp(path, "/") == 0) {
        get_file_list_request(sockfd);
        return;
    }
    if (strncmp(path, "/chunk", 7) == 0) {
        char chunk_name[1024];
        sscanf(request, "/chunk %s", chunk_name);
        get_chunk_request(sockfd, chunk_name);
        return;
    }
}
