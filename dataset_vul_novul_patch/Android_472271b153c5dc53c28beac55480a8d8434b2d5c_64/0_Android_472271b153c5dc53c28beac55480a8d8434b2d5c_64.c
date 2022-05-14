 ssize_t socket_write(const socket_t *socket, const void *buf, size_t count) {
   assert(socket != NULL);
   assert(buf != NULL);
 
  return TEMP_FAILURE_RETRY(send(socket->fd, buf, count, MSG_DONTWAIT));
 }
