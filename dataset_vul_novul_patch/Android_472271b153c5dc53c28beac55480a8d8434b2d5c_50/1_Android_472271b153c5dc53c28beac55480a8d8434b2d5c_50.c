static void update_logging() {
 bool should_log = module_started &&
 (logging_enabled_via_api || stack_config->get_btsnoop_turned_on());

 if (should_log == is_logging)
 return;

  is_logging = should_log;
 if (should_log) {
    btsnoop_net_open();

 const char *log_path = stack_config->get_btsnoop_log_path();

 if (stack_config->get_btsnoop_should_save_last()) {
 char last_log_path[PATH_MAX];
      snprintf(last_log_path, PATH_MAX, "%s.%llu", log_path, btsnoop_timestamp());
 if (!rename(log_path, last_log_path) && errno != ENOENT)

         LOG_ERROR("%s unable to rename '%s' to '%s': %s", __func__, log_path, last_log_path, strerror(errno));
     }
 
    logfile_fd = open(log_path, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
     if (logfile_fd == INVALID_FD) {
       LOG_ERROR("%s unable to open '%s': %s", __func__, log_path, strerror(errno));
       is_logging = false;
       return;
     }
 
    write(logfile_fd, "btsnoop\0\0\0\0\1\0\0\x3\xea", 16);
   } else {
     if (logfile_fd != INVALID_FD)
       close(logfile_fd);

    logfile_fd = INVALID_FD;
    btsnoop_net_close();
 }
}
