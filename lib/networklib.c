#include <errno.h>
#include <string.h>

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include "request.h"
#include "response.h"

#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>

int socket_listen(const char *host, const char *port) {
    /* Lookup server address information */
    struct addrinfo  hints = {
        .ai_family   = AF_UNSPEC,   /* Return IPv4 and IPv6 choices */
        .ai_socktype = SOCK_STREAM, /* Use TCP */
        .ai_flags    = AI_PASSIVE,  /* Use all interfaces */
    };
    struct addrinfo *results;
    int status;
    if ((status = getaddrinfo(host, port, &hints, &results)) != 0) {
    	fprintf(stderr, "getaddrinfo failed: %s\n", gai_strerror(status));
	return -1;
    }

    /* For each server entry, allocate socket and try to connect */
    int server_fd = -1;
    for (struct addrinfo *p = results; p != NULL && server_fd < 0; p = p->ai_next) {
	/* Allocate socket */
	if ((server_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0) {
	    fprintf(stderr, "Unable to make socket: %s\n", strerror(errno));
	    continue;
	}

	/* Bind socket */
	if (bind(server_fd, p->ai_addr, p->ai_addrlen) < 0) {
	    fprintf(stderr, "Unable to bind: %s\n", strerror(errno));
	    close(server_fd);
	    server_fd = -1;
	    continue;
	}

    	/* Listen to socket */
	if (listen(server_fd, SOMAXCONN) < 0) {
	    fprintf(stderr, "Unable to listen: %s\n", strerror(errno));
	    close(server_fd);
	    server_fd = -1;
	    continue;
	}
    }

    /* Release allocate address information */
    freeaddrinfo(results);

    return server_fd;
}

FILE *accept_client(int server_fd) {
    struct sockaddr client_addr;
    socklen_t client_len;

    /* Accept incoming connection */
    int client_fd = accept(server_fd, &client_addr, &client_len);
    if (client_fd < 0) {
        fprintf(stderr, "Unable to accept: %s\n", strerror(errno));
        return NULL;
    }

    /* Open file stream from socket file descriptor */
    FILE *client_file = fdopen(client_fd, "w+");
    if (client_file == NULL) {
        fprintf(stderr, "Unable to fdopen: %s\n", strerror(errno));
        close(client_fd);
    }

    return client_file;
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
