#pragma once
#include "request.h"
#include "response.h"
typedef struct state {
  struct user * head;
} state;

struct user {
  int session_id;
  int twidth;
  int theight;
  int in_fd;
  int out_fd;
  int pid;
  struct user * next;
};

state * init_state();
struct user * find_user(state * s, int id);
int start_user(state * s, int id);
enum error_type update_user(state * s, request * req, response * res);
void delete_state(state * s);
int new_user(state * s, int twidth, int theight);
void cleanup_user(struct user * u);
int delete_user(state * s, int target_id);
