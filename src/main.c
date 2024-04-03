#include <string.h>
#include "client.h"
#include "server.h"

int main(int argc, char *argv[]) {
    if (argc == 2 && strcmp("server", argv[1]) == 0) {
        return run_server();
    }

    return client(argc, argv);
}
