  InotifyReaderTask(InotifyReader* reader, int inotify_fd, int shutdown_fd)
       : reader_(reader),
         inotify_fd_(inotify_fd),
         shutdown_fd_(shutdown_fd) {
   }
