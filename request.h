struct request {
  enum request_type type;
  union request_content content;
};


enum request_type {
  REQCONNECT,
  REQUPDATE,
  REQDISCONNECT
};


union request_content {
  struct connect_request connect;
  struct update_request update;
  struct disconnect_request disconnect;
};


struct connect_request {
  int width;
  int height;
};


struct update_request {
  char buffer[128];
  int session_id;
};


struct disconnect_request {
  int session_id;
};
