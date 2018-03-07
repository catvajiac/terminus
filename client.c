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
int session_id;

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

    //Send connect request
    if (fwrite(req, sizeof(request), 1, client_file) < 0)
        perror("REQCONNECT");
    
    //Grab server response
    fread(res, sizeof(response), 1, client_file);

    if (res->type == RESCONNECT)
        session_id = res->content.connect.session_id;
    else
        perror("RESCONNECT");

    char out_buffer[OUT_BUFSIZ];
    char in_buffer[IN_BUFSIZ];
    while(1){

        bzero(in_buffer, IN_BUFSIZ);
        bzero(out_buffer, OUT_BUFSIZ);

        fgets(in_buffer, IN_BUFSIZ, stdin);
        
        //Set request fields
        req->type = REQUPDATE;
        int size = snprintf(req->content.update.buffer, sizeof(in_buffer), "%s", in_buffer);
        req->content.update.session_id = session_id;
        req->content.update.length = size;

        printf("%d",req->content.update.length);
        if (fwrite(req, sizeof(request), 1, client_file) < 0)
            perror("REQUPDATE");

        //fread(res, sizeof(response), 1, client_file);

        //Check again to see if there's more data in the buffer
        while(res->content.update.length == OUT_BUFSIZ){
            
        }

    }


    fclose(client_file);
    return EXIT_SUCCESS;
}


/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */
