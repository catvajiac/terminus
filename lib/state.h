typedef struct state {
  struct user * head;
} state;

struct user {
  int session_id;
  struct user * next;
};

state * init_state();
int new_user();
void delete_state(state * s);
int new_user(state * s);
void cleanup_user(struct user * u);
int delete_user(state * s, int target_id);
