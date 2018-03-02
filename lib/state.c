#include "state.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

state * init_state() {
  state * s = malloc(sizeof(state));
  if (!s) return NULL;
  s->head = NULL;
  return s;
}

void delete_state(state * s) {
  struct user * curr = s->head;
  struct user * temp;
  while (curr) {
    temp = curr->next;
    free(curr);
    curr = temp;
  }
}

int new_user(state * s, int width, int height) {
  struct user * u = malloc(sizeof(struct user));
  if (!u) return -1;
  u->next = s->head;
  u->session_id = s->head ? s->head->session_id + 1 : 0;
  u->in_fd = -1;
  u->out_fd = -1;
  u->twidth = width;
  u->theight = height;
  s->head = u;
  return u->session_id;
}

void cleanup_user(struct user * u) {
  //Once struct user gets more complex, this handles cleaning it up.
  if (u->in_fd >= 0) {
    close(u->in_fd);
  }
  if (u->out_fd >= 0) {
    close(u->out_fd);
  }
  free(u);
}

struct user * find_user(state * s, int id) {
  struct user * u = s->head;
  while (u && u->session_id != id) {
    u = u->next;
  }
  return u;
}

int delete_user(state * s, int target_id) {
  struct user * curr = s->head;
  if (curr && curr->session_id == target_id) {
    cleanup_user(curr);
    s->head = curr->next;
  }

  struct user * next = curr->next;
  while (curr && next && next->session_id != target_id) {
    curr = next;
    next = next->next;
  }
  if (!next) return -1;
  curr->next = next->next;
  cleanup_user(next);
  return 0;
}

int start_user(state * s, int id) {
  int rc = fork();
  int inpipe[2];
  int outpipe[2];
  struct user * u = find_user(s, id);
  if (pipe(inpipe) == -1) {
    printf("Error with pipe\n");
    return -1;
  }
  if (pipe(outpipe) == -1) {
    printf("Error with pipe\n");
    return -1;
  }
  if (rc < 0) {
    printf("Error forking\n");
    return -1;
  } else if (rc > 0) { //Parent
    close(inpipe[0]); //We don't need to read child's stdin
    close(outpipe[1]); //We don't need to write child's stdout
    u->in_fd = inpipe[1];
    u->out_fd = outpipe[0];
  } else { //Child
    dup2(inpipe[0], 0); //Read from the read end on stdin
    dup2(outpipe[1], 1); //Write to the write end on stdout
    close(inpipe[0]);
    close(inpipe[1]);
    close(outpipe[0]);
    close(outpipe[1]);
    char wstr[20];
    char hstr[20];
    sprintf(wstr, "%i", u->twidth);
    sprintf(hstr, "%i", u->theight);
    char * argv[] = {"./apps/menu", "-H", hstr, "-W", wstr};
    execvp("./apps/menu", argv);
  }
  return 0;
}

enum error_type update_user(state * s, request * req, response * res) {
  struct user * u = find_user(s, req->content.update.session_id);
  int w;
  int w_target = req->content.update.length;
  if (w_target > 0 && w_target < IN_BUFSIZ) {
    w = write(u->in_fd, req->content.update.buffer, w_target);
  } else if (w_target < 0 || w_target >= IN_BUFSIZ) {
    return ERPARAM;
  }
  if (w != w_target) {
    return ERINTERNAL;
  }
  int r = read(u->out_fd, res->content.update.buffer, OUT_BUFSIZ);
  if ( r >= 0) {
    res->content.update.length = r;
  } else {
    return ERINTERNAL;
  }
  return NOERR;
}
