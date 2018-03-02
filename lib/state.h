typedef struct state {
  struct user * head;
} state;

struct user {
  int session_id;
  int twidth;
  int theight;
  struct user * next;
};

state * init_state();
struct user * find_user(state * s, int id);
void delete_state(state * s);
int new_user(state * s, int twidth, int theight);
void cleanup_user(struct user * u);
int delete_user(state * s, int target_id);
