static void *listen_fn_(UNUSED_ATTR void *context) {

  prctl(PR_SET_NAME, (unsigned long)LISTEN_THREAD_NAME_, 0, 0, 0);

  listen_socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
 if (listen_socket_ == -1) {
    LOG_ERROR("%s socket creation failed: %s", __func__, strerror(errno));
 goto cleanup;
 }

 int enable = 1;
 if (setsockopt(listen_socket_, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) == -1) {
    LOG_ERROR("%s unable to set SO_REUSEADDR: %s", __func__, strerror(errno));
 goto cleanup;
 }

 struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(LOCALHOST_);
  addr.sin_port = htons(LISTEN_PORT_);
 if (bind(listen_socket_, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
    LOG_ERROR("%s unable to bind listen socket: %s", __func__, strerror(errno));
 goto cleanup;
 }

 if (listen(listen_socket_, 10) == -1) {
    LOG_ERROR("%s unable to listen: %s", __func__, strerror(errno));
 goto cleanup;

   }
 
   for (;;) {
    int client_socket = TEMP_FAILURE_RETRY(accept(listen_socket_, NULL, NULL));
     if (client_socket == -1) {
       if (errno == EINVAL || errno == EBADF) {
         break;
 }
      LOG_WARN("%s error accepting socket: %s", __func__, strerror(errno));
 continue;
 }

 /* When a new client connects, we have to send the btsnoop file header. This allows
       a decoder to treat the session as a new, valid btsnoop file. */

     pthread_mutex_lock(&client_socket_lock_);
     safe_close_(&client_socket_);
     client_socket_ = client_socket;
    TEMP_FAILURE_RETRY(send(client_socket_, "btsnoop\0\0\0\0\1\0\0\x3\xea", 16, 0));
     pthread_mutex_unlock(&client_socket_lock_);
   }
 
cleanup:
  safe_close_(&listen_socket_);
 return NULL;
}
