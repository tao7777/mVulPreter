bool Channel::ChannelImpl::ProcessOutgoingMessages() {
  DCHECK(!waiting_connect_);  // Why are we trying to send messages if there's
  is_blocked_on_write_ = false;

  if (output_queue_.empty()) {
    return true;
  }

  if (pipe_ == -1) {
    return false;
  }

  while (!output_queue_.empty()) {
    Message* msg = output_queue_.front();

#if !defined(OS_MACOSX)
    scoped_ptr<Message> hello;
    if (remote_fd_pipe_ != -1 &&
        msg->routing_id() == MSG_ROUTING_NONE &&
        msg->type() == HELLO_MESSAGE_TYPE) {
      hello.reset(new Message(MSG_ROUTING_NONE,
                              HELLO_MESSAGE_TYPE,
                              IPC::Message::PRIORITY_NORMAL));
      void* iter = NULL;
      int pid;
      if (!msg->ReadInt(&iter, &pid) ||
          !hello->WriteInt(pid)) {
        NOTREACHED();
      }
      DCHECK_EQ(hello->size(), msg->size());
      if (!hello->WriteFileDescriptor(base::FileDescriptor(remote_fd_pipe_,
                                                           false))) {
        NOTREACHED();
      }
      msg = hello.get();
      DCHECK_EQ(msg->file_descriptor_set()->size(), 1);
    }
#endif

    size_t amt_to_write = msg->size() - message_send_bytes_written_;
    DCHECK(amt_to_write != 0);
    const char* out_bytes = reinterpret_cast<const char*>(msg->data()) +
        message_send_bytes_written_;

    struct msghdr msgh = {0};
    struct iovec iov = {const_cast<char*>(out_bytes), amt_to_write};
    msgh.msg_iov = &iov;
    msgh.msg_iovlen = 1;
    char buf[CMSG_SPACE(
        sizeof(int[FileDescriptorSet::MAX_DESCRIPTORS_PER_MESSAGE]))];

    ssize_t bytes_written = 1;
    int fd_written = -1;

    if (message_send_bytes_written_ == 0 &&
        !msg->file_descriptor_set()->empty()) {
      struct cmsghdr *cmsg;
       const unsigned num_fds = msg->file_descriptor_set()->size();
 
       DCHECK_LE(num_fds, FileDescriptorSet::MAX_DESCRIPTORS_PER_MESSAGE);
      if (msg->file_descriptor_set()->ContainsDirectoryDescriptor()) {
        LOG(FATAL) << "Panic: attempting to transport directory descriptor over"
                      " IPC. Aborting to maintain sandbox isolation.";
        // If you have hit this then something tried to send a file descriptor
        // to a directory over an IPC channel. Since IPC channels span
        // sandboxes this is very bad: the receiving process can use openat
        // with ".." elements in the path in order to reach the real
        // filesystem.
      }
 
       msgh.msg_control = buf;
       msgh.msg_controllen = CMSG_SPACE(sizeof(int) * num_fds);
      cmsg = CMSG_FIRSTHDR(&msgh);
      cmsg->cmsg_level = SOL_SOCKET;
      cmsg->cmsg_type = SCM_RIGHTS;
      cmsg->cmsg_len = CMSG_LEN(sizeof(int) * num_fds);
      msg->file_descriptor_set()->GetDescriptors(
          reinterpret_cast<int*>(CMSG_DATA(cmsg)));
      msgh.msg_controllen = cmsg->cmsg_len;

      msg->header()->num_fds = static_cast<uint16>(num_fds);

#if !defined(OS_MACOSX)
      if (!uses_fifo_ &&
          (msg->routing_id() != MSG_ROUTING_NONE ||
           msg->type() != HELLO_MESSAGE_TYPE)) {
        struct iovec fd_pipe_iov = { const_cast<char *>(""), 1 };
        msgh.msg_iov = &fd_pipe_iov;
        fd_written = fd_pipe_;
        bytes_written = HANDLE_EINTR(sendmsg(fd_pipe_, &msgh, MSG_DONTWAIT));
        msgh.msg_iov = &iov;
        msgh.msg_controllen = 0;
        if (bytes_written > 0) {
          msg->file_descriptor_set()->CommitAll();
        }
      }
#endif
    }

    if (bytes_written == 1) {
      fd_written = pipe_;
#if !defined(OS_MACOSX)
      if (mode_ != MODE_SERVER && !uses_fifo_ &&
          msg->routing_id() == MSG_ROUTING_NONE &&
          msg->type() == HELLO_MESSAGE_TYPE) {
        DCHECK_EQ(msg->file_descriptor_set()->size(), 1);
      }
      if (!uses_fifo_ && !msgh.msg_controllen) {
        bytes_written = HANDLE_EINTR(write(pipe_, out_bytes, amt_to_write));
      } else
#endif
      {
        bytes_written = HANDLE_EINTR(sendmsg(pipe_, &msgh, MSG_DONTWAIT));
      }
    }
    if (bytes_written > 0)
      msg->file_descriptor_set()->CommitAll();

    if (bytes_written < 0 && !SocketWriteErrorIsRecoverable()) {
#if defined(OS_MACOSX)
      if (errno == EPERM) {
        Close();
        return false;
      }
#endif  // OS_MACOSX
      if (errno == EPIPE) {
        Close();
        return false;
      }
      PLOG(ERROR) << "pipe error on "
                  << fd_written
                  << " Currently writing message of size:"
                  << msg->size();
      return false;
    }

    if (static_cast<size_t>(bytes_written) != amt_to_write) {
      if (bytes_written > 0) {
        message_send_bytes_written_ += bytes_written;
      }

      is_blocked_on_write_ = true;
      MessageLoopForIO::current()->WatchFileDescriptor(
          pipe_,
          false,  // One shot
          MessageLoopForIO::WATCH_WRITE,
          &write_watcher_,
          this);
      return true;
    } else {
      message_send_bytes_written_ = 0;

#ifdef IPC_MESSAGE_DEBUG_EXTRA
      DLOG(INFO) << "sent message @" << msg << " on channel @" << this <<
                    " with type " << msg->type();
#endif
      delete output_queue_.front();
      output_queue_.pop();
    }
  }
  return true;
}
