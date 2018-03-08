#include <stdlib.h>
#include <stdio.h>
#include "request.h"
#include "response.h"
#include "networklib.h"
#include "state.h"
#include "signal.h"
#include "server_handlers.h"

response * handle_connect(request * req, state * s);
response * handle_update(request * req, state * s);
response * handle_disconnect(request * req, state * s);
response * handle_error(enum error_type e);
void handle_request(state * user_states, int server_fd);
request * get_request(FILE * client_file);
response * interpret_request(request * req, state * s);
void send_response(response * res, FILE * client_file);
void cleanup(request * req, response * res);
void cleanup_server();
state * user_states;

int handle_requests(const char * HOST, const char * PORT) {
  int server_fd = socket_listen(HOST, PORT);
  if (server_fd < 0) {
    return 1;
  }
  signal(SIGTERM, cleanup_server);
  //signal(SIGINT, cleanup_server);
  user_states = init_state();
  while (1) {
    handle_request(user_states, server_fd);
  }
}

void handle_request(state * s, int server_fd) {
  FILE * client_file = accept_client(server_fd);
  if (client_file == NULL) return;

  request * req = get_request(client_file);
  response * res = interpret_request(req, s);
  send_response(res, client_file);
  cleanup(req, res);
  fclose(client_file);
}

void cleanup(request * req, response * res) {
  if (req) {
    free(req);
  }
  if (res) {
    free(res);
  }
}

request * get_request(FILE * client_file) {
  printf("enter get_req\n");
  request * req = malloc(sizeof(request));
  if (!req) return NULL;

  while (fread((char *)req, sizeof(request), 1, client_file) != 1) continue;

  return req;
}

void send_response(response * res, FILE * client_file) {
  printf("enter send_resp\n");
  if (!res) {
    printf("  ! send_response received NULL\n");
    return;
  }
  printf("about to send_resp\n");
  fwrite((char *)res, sizeof(response), 1, client_file);
  printf("response sent!\n");
}

response * interpret_request(request * req, state * s) {
  printf("enter interp_req\n");
  if (!req) {
    printf("  ! interpret_request received NULL\n");
    return NULL;
  }
  switch (req->type) {
    case REQCONNECT:
      return handle_connect(req, s);
    case REQUPDATE:
      return handle_update(req, s);
    case REQDISCONNECT:
      return handle_disconnect(req, s);
    default:
      return handle_error(ERNOTYPE);
  }
}

response * handle_error(enum error_type e) {
  printf("enter handle_error\n");
  response * res = malloc(sizeof(response));
  if (!res) return NULL;
  res->type = RESERROR;
  res->content.error.error = e;
  return res;
}

response * handle_update(request * req, state * s) {
  printf("enter handle_update\n");
  struct user * u = find_user(s, req->content.update.session_id);
  if (!u) {
    printf("  ! user not found\n");
    return handle_error(ERINVALIDSESSION);
  }
  response * res = malloc(sizeof(response));
  if (!res) {
    printf("  ! malloc failure\n");
    return NULL;
  }
  res->type = RESUPDATE;
  enum error_type e = update_user(s, req, res);
  if (e != NOERR) {
    printf("  ! update user failure\n");
    free(res);
    return handle_error(e);
  }
  return res;
}

response * handle_disconnect(request * req, state * s) {
  printf("enter handle_disconnect\n");
  struct user * u = find_user(s, req->content.update.session_id);
  if (!u) return handle_error(ERINVALIDSESSION);
  response * res = malloc(sizeof(response));
  if (!res) return NULL;
  res->type = RESDISCONNECT;
  return res;
}

response * handle_connect(request * req, state * s) {
  printf("enter handle_connect\n");
  int id;
  if ((id = new_user(s, req->content.connect.width, req->content.connect.height)) < 0) {
    printf("  ! new_user error\n");
    return handle_error(ERINTERNAL);
  }
  response * res = malloc(sizeof(response));
  if (!res) {
    printf("  ! malloc failure handle_connect\n");
    return NULL;
  }
  res->type = RESCONNECT;
  res->content.connect.session_id = id;
  if (start_user(s, id) < 0) {
    free(res);
    printf("  ! start user failed\n");
    return handle_error(ERINTERNAL);
  }
  return res;
}

void cleanup_server() {
  delete_state(user_states);
}
