char* engrave_tombstone(pid_t pid, pid_t tid, int signal, int original_si_code,
 uintptr_t abort_msg_address, bool dump_sibling_threads,
 bool* detach_failed, int* total_sleep_time_usec) {

 log_t log;

   log.current_tid = tid;
   log.crashed_tid = tid;
 
   int fd = -1;
  char* path = find_and_open_tombstone(&fd);
 
   if (fd < 0) {
     _LOG(&log, logtype::ERROR, "Skipping tombstone write, nothing to do.\n");
 *detach_failed = false;
 return NULL;
 }

  log.tfd = fd;
 int amfd = activity_manager_connect();
  log.amfd = amfd;
 *detach_failed = dump_crash(&log, pid, tid, signal, original_si_code, abort_msg_address,
                              dump_sibling_threads, total_sleep_time_usec);

  ALOGI("\nTombstone written to: %s\n", path);

  close(amfd);
  close(fd);

 return path;
}
