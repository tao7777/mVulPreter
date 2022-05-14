ssize_t socket_read(const socket_t *socket, void *buf, size_t count) {

   assert(socket != NULL);
   assert(buf != NULL);
 
  return TEMP_FAILURE_RETRY(recv(socket->fd, buf, count, MSG_DONTWAIT));
 }
