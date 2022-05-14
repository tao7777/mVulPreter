 static void btsnoop_write(const void *data, size_t length) {
   if (logfile_fd != INVALID_FD)
    TEMP_FAILURE_RETRY(write(logfile_fd, data, length));
 
   btsnoop_net_write(data, length);
 }
