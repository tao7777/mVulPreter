void SendStatus(struct mg_connection* connection,
void SendOkWithBody(struct mg_connection* connection,
                    const std::string& content) {
  const char* response_fmt = "HTTP/1.1 200 OK\r\n"
                             "Content-Length:%d\r\n\r\n"
                             "%s";
  std::string response = base::StringPrintf(
      response_fmt, content.length(), content.c_str());
   mg_write(connection, response.data(), response.length());
 }
