#include <stdlib.h>
#include "lib/server_handlers.h"

const char *HOST = NULL;
const char *PORT = "9432";

int main(int argc, char *argv[]) {
    handle_requests(HOST, PORT);
    return EXIT_SUCCESS;
}
/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */
