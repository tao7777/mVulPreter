  void SendRendererReply(const std::vector<int>& fds, const Pickle& reply,
                         int reply_fd) {
    struct msghdr msg;
    memset(&msg, 0, sizeof(msg));
    struct iovec iov = {const_cast<void*>(reply.data()), reply.size()};
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

     char control_buffer[CMSG_SPACE(sizeof(int))];
 
     if (reply_fd != -1) {
      struct stat st;
      if (fstat(reply_fd, &st) == 0 && S_ISDIR(st.st_mode)) {
        LOG(FATAL) << "Tried to send a directory descriptor over sandbox IPC";
        // We must never send directory descriptors to a sandboxed process
        // because they can use openat with ".." elements in the path in order
        // to escape the sandbox and reach the real filesystem.
      }
 
      struct cmsghdr *cmsg;
       msg.msg_control = control_buffer;
       msg.msg_controllen = sizeof(control_buffer);
       cmsg = CMSG_FIRSTHDR(&msg);
      cmsg->cmsg_level = SOL_SOCKET;
      cmsg->cmsg_type = SCM_RIGHTS;
      cmsg->cmsg_len = CMSG_LEN(sizeof(int));
      memcpy(CMSG_DATA(cmsg), &reply_fd, sizeof(reply_fd));
      msg.msg_controllen = cmsg->cmsg_len;
    }

    if (HANDLE_EINTR(sendmsg(fds[0], &msg, MSG_DONTWAIT)) < 0)
      PLOG(ERROR) << "sendmsg";
  }
