 socket_t *socket_accept(const socket_t *socket) {
   assert(socket != NULL);
 
  int fd = TEMP_FAILURE_RETRY(accept(socket->fd, NULL, NULL));
   if (fd == INVALID_FD) {
     LOG_ERROR("%s unable to accept socket: %s", __func__, strerror(errno));
     return NULL;
 }

 socket_t *ret = (socket_t *)osi_calloc(sizeof(socket_t));
 if (!ret) {
    close(fd);
    LOG_ERROR("%s unable to allocate memory for socket.", __func__);
 return NULL;
 }

  ret->fd = fd;
 return ret;
}
