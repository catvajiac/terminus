struct update_response {
  int length;
  char buffer[1024];
};


enum error_type {
  ERINVALIDSESSION,
  ERINTERNAL,
  ERNOTYPE,
  EROTHER
};


struct error_response {
  enum error_type error;
};


struct connect_response {
  int session_id;
};


enum response_type {
  RESCONNECT,
  RESDISCONNECT,
  RESUPDATE,
  RESERROR
};


union response_content {
  struct update_response update;
  struct connect_response connect;
  struct error_response error;
  // No content needed for disconnect, the request's type is confirmation.
};


typedef struct response {
  enum response_type type;
  union response_content content;
} response;
