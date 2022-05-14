static void uv__process_child_init(const uv_process_options_t* options,
                                   int stdio_count,
                                   int (*pipes)[2],
                                   int error_fd) {
  int close_fd;
  int use_fd;
  int fd;

  if (options->flags & UV_PROCESS_DETACHED)
    setsid();

  for (fd = 0; fd < stdio_count; fd++) {
    close_fd = pipes[fd][0];
    use_fd = pipes[fd][1];

    if (use_fd < 0) {
      if (fd >= 3)
        continue;
      else {
        /* redirect stdin, stdout and stderr to /dev/null even if UV_IGNORE is
         * set
         */
        use_fd = open("/dev/null", fd == 0 ? O_RDONLY : O_RDWR);
        close_fd = use_fd;

        if (use_fd == -1) {
        uv__write_int(error_fd, -errno);
          perror("failed to open stdio");
          _exit(127);
        }
      }
    }

    if (fd == use_fd)
      uv__cloexec(use_fd, 0);
    else
      dup2(use_fd, fd);

    if (fd <= 2)
      uv__nonblock(fd, 0);

    if (close_fd != -1)
      uv__close(close_fd);
  }

  for (fd = 0; fd < stdio_count; fd++) {
    use_fd = pipes[fd][1];

    if (use_fd >= 0 && fd != use_fd)
      close(use_fd);
  }

  if (options->cwd != NULL && chdir(options->cwd)) {
    uv__write_int(error_fd, -errno);
    perror("chdir()");
     _exit(127);
   }
 
  if (options->flags & (UV_PROCESS_SETUID | UV_PROCESS_SETGID)) {
    /* When dropping privileges from root, the `setgroups` call will
     * remove any extraneous groups. If we don't call this, then
     * even though our uid has dropped, we may still have groups
     * that enable us to do super-user things. This will fail if we
     * aren't root, so don't bother checking the return value, this
     * is just done as an optimistic privilege dropping function.
     */
    SAVE_ERRNO(setgroups(0, NULL));
  }

   if ((options->flags & UV_PROCESS_SETGID) && setgid(options->gid)) {
     uv__write_int(error_fd, -errno);
     perror("setgid()");
    _exit(127);
  }

  if ((options->flags & UV_PROCESS_SETUID) && setuid(options->uid)) {
    uv__write_int(error_fd, -errno);
    perror("setuid()");
    _exit(127);
  }

  if (options->env != NULL) {
    environ = options->env;
  }

  execvp(options->file, options->args);
  uv__write_int(error_fd, -errno);
  perror("execvp()");
  _exit(127);
}
