static inline int accept_server_socket(int s)

 {
     struct sockaddr_un client_address;
     socklen_t clen;
    int fd = TEMP_FAILURE_RETRY(accept(s, (struct sockaddr*)&client_address, &clen));
     APPL_TRACE_DEBUG("accepted fd:%d for server fd:%d", fd, s);
     return fd;
 }
