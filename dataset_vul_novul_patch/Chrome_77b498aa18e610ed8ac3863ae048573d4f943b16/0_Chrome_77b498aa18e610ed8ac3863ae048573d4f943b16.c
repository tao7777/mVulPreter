  InotifyReaderTask(InotifyReader* reader, int inotify_fd, int shutdown_fd)
       : reader_(reader),
         inotify_fd_(inotify_fd),
         shutdown_fd_(shutdown_fd) {
    // Make sure the file descriptors are good for use with select().
    CHECK_LE(0, inotify_fd_);
    CHECK_GT(FD_SETSIZE, inotify_fd_);
    CHECK_LE(0, shutdown_fd_);
    CHECK_GT(FD_SETSIZE, shutdown_fd_);
   }
