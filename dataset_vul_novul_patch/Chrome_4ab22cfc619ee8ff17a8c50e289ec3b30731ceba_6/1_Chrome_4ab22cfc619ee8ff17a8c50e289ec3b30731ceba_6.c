void SendStatus(struct mg_connection* connection,
                const struct mg_request_info* request_info,
                void* user_data) {
  std::string response = "HTTP/1.1 200 OK\r\n"
                         "Content-Length:2\r\n\r\n"
                         "ok";
   mg_write(connection, response.data(), response.length());
 }
