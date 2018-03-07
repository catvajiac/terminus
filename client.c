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
int session_id, char_in = 0; //SID and size of read in

void* getKeys(void* arg);


/*
  Thread helper functions
*/
void start(void* func, void *arg, pthread_t thread){
    int retval = pthread_create(&thread, NULL, func, arg);
    if (retval != 0){
        perror("create");
    }
}

void join(void **result, pthread_t thread){
    int retval = pthread_join(thread, result);
    //The Join operation failed
    if (retval != 0){
        perror("join");
    }
}

void detach(pthread_t thread){
    int retval = pthread_detach(thread);
    if (retval != 0){
        perror("detach");
    }
}


int main(int argc, char *argv[]) {

    //Retreive size of client terminal in terms of rows and cols
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    /* Connect to remote machine */
    FILE * client_file = socket_dial(HOST, PORT);
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
    char *circle_buffer = malloc(4096); //God help us all

    pthread_t keys;
    start(getKeys, circle_buffer, keys);
    detach(keys);

    int size, char_out = 0;

    while(1){
        do {

            //Put the stuff in the req
            size = snprintf(req->content.update.buffer, (char_in - char_out), "%s", circle_buffer);
            req->content.update.length = size;
            req->content.update.session_id = session_id;

            if (fwrite(req, sizeof(request), 1, client_file) < 0)
                perror("REQCONNECT");
            
            //Update size of lagging pointer
            char_out += IN_BUFSIZ;
            if(char_out > char_in)
                char_out = char_in;
        } while((char_out - char_in) >= IN_BUFSIZ);


        //Get response
        fread(res, sizeof(response), 1, client_file);
    }

    fclose(client_file);
    return EXIT_SUCCESS;
}

void *getKeys(void* arg){
    char* buffer = (char *) arg;
    char in_buffer[IN_BUFSIZ];

    int read_size = fread(in_buffer, IN_BUFSIZ, 1, stdin);
    
    if(char_in + read_size <= 4096){
        buffer[char_in] = in_buffer;
        char_in += read_size;
    }
    else{
        //Split the string
        int overflow = (char_in + read_size) - 4096;
        char * first, second;
        strncpy(first, in_buffer, (read_size - overflow));
        strncpy(second, in_buffer[char_in], overflow);

        buffer[char_in] = first;
        buffer[0] = second;
        char_in = (char_in + IN_BUFSIZ) % 4096; //Wraparound
    }
}

/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */
