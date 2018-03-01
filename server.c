/* echo_server_refactored.c: simple TCP echo server  */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include "lib/request.h"
#include "lib/response.h"
#include "lib/networklib.h"

const char *HOST = NULL;
const char *PORT = "9432";

int main(int argc, char *argv[]) {
    int server_fd = socket_listen(HOST, PORT);
    if (server_fd < 0) {
        return EXIT_FAILURE;
    }

    /* Process incoming connections */
    int sid = 0;
    while (1) {
        FILE *client_file = accept_client(server_fd);
        if (client_file == NULL) {
            continue;
        }

        /* Read from client and then echo back */
        request * req = malloc(sizeof(request));
        response * res = malloc(sizeof(response));
        res->type = RESCONNECT;
        res->content.connect.session_id = sid;
        sid += 1;
        while (fread((char *)req, sizeof(request), 1, client_file)) {
            printf("Received a connect request\n");
            fwrite((char *)res, sizeof(response), 1, client_file);
        }

        /* Close connection */
        fclose(client_file);
    }

    return EXIT_SUCCESS;
}
/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */
