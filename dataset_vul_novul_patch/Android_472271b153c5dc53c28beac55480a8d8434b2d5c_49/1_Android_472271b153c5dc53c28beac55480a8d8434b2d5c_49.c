 static void btsnoop_write(const void *data, size_t length) {
   if (logfile_fd != INVALID_FD)
    write(logfile_fd, data, length);
 
   btsnoop_net_write(data, length);
 }
