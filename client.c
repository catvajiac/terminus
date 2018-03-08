/* echo_client.c: simple TCP echo client */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include "lib/request.h"
#include "lib/response.h"
#include "lib/networklib.h"
#include <sys/ioctl.h>

#define LOCAL_BUFSIZ 4096

int delay = 100000; //useconds
const char *HOST = "localhost";
const char *PORT = "9432";
int char_in = 0; // Leading/reading from keyboard
int char_out = 0; // Lagging/writing to request
char * circle_buffer;

void* getKeys();
int get_session(FILE* client_file);
request * make_request(int session_id);
response * send_request(request * req, FILE * client_file);
void interpret_res(response * res);

int main(int argc, char *argv[]) {
  /* Connect to remote machine */
  FILE * client_file = socket_dial(HOST, PORT);
  if (client_file == NULL) {
    printf("Error connecting!\n");
    return EXIT_FAILURE;
  }
  int session_id = get_session(client_file);

  circle_buffer = malloc(LOCAL_BUFSIZ); //God help us all
  if (!circle_buffer) {
    printf("Buffer declaration failure!\n");
    return EXIT_FAILURE;
  }

  pthread_t keys;
  if (pthread_create(&keys, NULL, getKeys, NULL)) {
    printf("Threads dead baby!\n");
  }
  while (1) {
    request * req = make_request(session_id);
    if (req) {
      response * res = send_request(req, client_file);
      interpret_res(res); //Delays only if res is not full
    }
  }
  fclose(client_file);
  return EXIT_SUCCESS;
}

void *getKeys(){
  char in_buffer[IN_BUFSIZ];

  while(1){
    int read_size = fread(in_buffer, IN_BUFSIZ, 1, stdin);
    int i = 0;
    while (read_size) {
      circle_buffer[char_in] = in_buffer[i];
      char_in = (char_in + 1) % LOCAL_BUFSIZ;
    }
  }
}

int get_session(FILE* client_file) {
    //Make new request and check and stuff
    request req;
    response res;
    
    //Send connection
    req.type = REQCONNECT;

    //Retreive size of client terminal in terms of rows and cols
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    req.content.connect.width = w.ws_col;
    req.content.connect.height = w.ws_row;

    //Send connect request
    if (fwrite(&req, sizeof(request), 1, client_file) < 0)
        perror("REQCONNECT");
    
    //Grab server response
    fread(&res, sizeof(response), 1, client_file);

    if (res.type == RESCONNECT) {
        int sid = res.content.connect.session_id;
        printf("Our session id: %d\n", sid);
        return sid;
    }
    else {
        printf("Error connecting to server");
        exit(1);
    }
}

request * make_request(int session_id) {
  request * r = malloc(sizeof(request));
  if (!r) {
    printf("Malloc bad\n");
    exit(1);
  }
  r->type = REQUPDATE;
  r->content.update.session_id = session_id;
  int length = 0;
  while (char_in != char_out && length < IN_BUFSIZ) {
    r->content.update.buffer[length] = circle_buffer[char_out];
    char_out = (char_out + 1) % LOCAL_BUFSIZ;
    length += 1;
  }
  r->content.update.length = length;
  return r;
}

response * send_request(request * req, FILE * client_file) {
  if (fwrite(&req, sizeof(request), 1, client_file) < 0)
    printf("fwrite not lookin good\n");
            
  response * res = malloc(sizeof(response));
  if (!res) {
    printf("Malloc bad\n");
    return NULL;
  }
  fread((void *)res, sizeof(response), 1, client_file);
  return res;
}

void interpret_res(response * res) {
  if (!res) {
    printf("No such response.  fatal.\n");
    exit(1);
  }
  if (res->type == RESERROR) {
    printf("Server didn't like that.  Server didn't like that one bit.\n");
    exit(1);
  }
  printf("%s\n", res->content.update.buffer);
  if (res->content.update.length != OUT_BUFSIZ) {
    usleep(delay);
  }
}
/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */
