void SendNotImplementedError(struct mg_connection* connection,
                             const struct mg_request_info* request_info,
                             void* user_data) {
  std::string body = base::StringPrintf(
      "{\"status\":%d,\"value\":{\"message\":"
      "\"Command has not been implemented yet: %s %s\"}}",
      kUnknownCommand, request_info->request_method, request_info->uri);

  std::string header = base::StringPrintf(
      "HTTP/1.1 501 Not Implemented\r\n"
      "Content-Type:application/json\r\n"
       "Content-Length:%" PRIuS "\r\n"
       "\r\n", body.length());
 
  LOG(ERROR) << header << body;
   mg_write(connection, header.data(), header.length());
   mg_write(connection, body.data(), body.length());
 }
