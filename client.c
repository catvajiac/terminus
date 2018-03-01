/* echo_client.c: simple TCP echo client */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include "request.h"
#include "response.h"

#include <sys/ioctl.h>

const char *HOST = "localhost";
const char *PORT = "9432";

FILE *socket_dial(const char *host, const char *port);

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

FILE *socket_dial(const char *host, const char *port) {
    /* Lookup server address information */
    struct addrinfo *results;
    struct addrinfo  hints = {
        .ai_family   = AF_UNSPEC,   /* Return IPv4 and IPv6 choices */
        .ai_socktype = SOCK_STREAM, /* Use TCP */
    };
    int status;
    if ((status = getaddrinfo(host, port, &hints, &results)) != 0) {
        fprintf(stderr, "getaddrinfo failed: %s\n", gai_strerror(status));
        return NULL;
    }

    /* For each server entry, allocate socket and try to connect */
    int client_fd = -1;
    for (struct addrinfo *p = results; p != NULL && client_fd < 0; p = p->ai_next) {
      /* Allocate socket */
      if ((client_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0) {
          fprintf(stderr, "Unable to make socket: %s\n", strerror(errno));
          continue;
      }

      /* Connect to host */
      if (connect(client_fd, p->ai_addr, p->ai_addrlen) < 0) {
          fprintf(stderr, "Unable to connect to %s:%s: %s\n", host, port, strerror(errno));
          close(client_fd);
          client_fd = -1;
          continue;
      }
    }

    /* Release allocate address information */
    freeaddrinfo(results);

    if (client_fd < 0) {
    	return NULL;
    }

    /* Open file stream from socket file descriptor */
    FILE *client_file = fdopen(client_fd, "w+");
    if (client_file == NULL) {
        fprintf(stderr, "Unable to fdopen: %s\n", strerror(errno));
        close(client_fd);
        return NULL;
    }

    return client_file;
}

/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */
