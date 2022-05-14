ssize_t socket_bytes_available(const socket_t *socket) {

   assert(socket != NULL);
 
   int size = 0;
  if (TEMP_FAILURE_RETRY(ioctl(socket->fd, FIONREAD, &size)) == -1)
     return -1;
   return size;
 }
