/* echo_client.c: simple TCP echo client */

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

#include <sys/ioctl.h>

const char *HOST = "localhost";
const char *PORT = "9432";

int main(int argc, char *argv[]) {

    //Retreive size of client terminal in terms of rows and cols
    //TODO: Poll in background for change in size to send updates
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    /* Connect to remote machine */
    FILE *client_file = socket_dial(HOST, PORT);
    if (client_file == NULL) {
        return EXIT_FAILURE;
    }

    /* Read from stdin and send to server */

    //Make new request and check and stuff
    request *req = malloc(sizeof(request));
    response *res = malloc(sizeof(response));
    if(!(req && res)){
        perror("REQ_CREATE");
    }

    //Send connection
    req->type = REQCONNECT;

    //Grab terminal size from client user
    req->content.connect.width = w.ws_col;
    req->content.connect.height = w.ws_row;

    if (fwrite(req, sizeof(request), 1, client_file) < 0) {
        printf("error!\n");
    }
    fread(res, sizeof(response), 1, client_file);
    if (res->type == RESCONNECT) {
        printf("My id is: %i\n", res->content.connect.session_id);
    } else {
        printf("error\n");
    }
    fclose(client_file);
    return EXIT_SUCCESS;
}


/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */
