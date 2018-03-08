#include <stdlib.h>
#include "lib/server_handlers.h"

const char *HOST = NULL;
const char *PORT = "9222";

int main(int argc, char *argv[]) {
    if (argc > 1) {
      PORT = argv[1];
    }
    handle_requests(HOST, PORT);
    return EXIT_SUCCESS;
}
/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */
