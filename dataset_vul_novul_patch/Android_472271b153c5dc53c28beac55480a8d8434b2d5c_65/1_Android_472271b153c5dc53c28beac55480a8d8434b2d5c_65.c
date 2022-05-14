 ssize_t socket_write_and_transfer_fd(const socket_t *socket, const void *buf, size_t count, int fd) {
  assert(socket != NULL);
  assert(buf != NULL);

 if (fd == INVALID_FD)
 return socket_write(socket, buf, count);

 struct msghdr msg;
 struct iovec iov;
 char control_buf[CMSG_SPACE(sizeof(int))];

  iov.iov_base = (void *)buf;
  iov.iov_len = count;

  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;
  msg.msg_control = control_buf;
  msg.msg_controllen = sizeof(control_buf);
  msg.msg_name = NULL;
  msg.msg_namelen = 0;

 struct cmsghdr *header = CMSG_FIRSTHDR(&msg);
  header->cmsg_level = SOL_SOCKET;
  header->cmsg_type = SCM_RIGHTS;

   header->cmsg_len = CMSG_LEN(sizeof(int));
   *(int *)CMSG_DATA(header) = fd;
 
  ssize_t ret = sendmsg(socket->fd, &msg, MSG_DONTWAIT);
   close(fd);
   return ret;
 }
