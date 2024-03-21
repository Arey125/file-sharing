#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "server.h"
#include "client.h"

int main(int argc, char *argv[]) {
    if (argc == 1) {
        return run_server();
    }

    if (argc > 1) {
        return client(argc, argv);
    }

    return 0;
}
