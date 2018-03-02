#include "state.h"
#include "stdlib.h"

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

int new_user(state * s) {
  struct user * u = malloc(sizeof(struct user));
  if (!u) return -1;
  u->next = s->head;
  u->session_id = s->head ? s->head->session_id + 1 : 0;
  s->head = u;
  return u->session_id;
}

void cleanup_user(struct user * u) {
  //Once struct user gets more complex, this handles cleaning it up.
  free(u);
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
