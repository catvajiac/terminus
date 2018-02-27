struct response {
  enum response_type type;
  union response_content content;
};


enum response_type {
  RESCONNECT,
  RESUPDATE,
  RESERROR
};

enum error_type {
  ERINVALIDSESSION,
  EROTHER
};


union response_content {
  struct update_response update;
  struct connect_response connect;
  struct error_response error;
};


struct update_response {
  int length;
  char buffer[1024];
};

struct connect_response {
  enum error_type error;
};

struct connect_response {
  int session_id;
};
