static void worker_process(int fd, debugger_request_t& request) {
  std::string tombstone_path;
 int tombstone_fd = -1;
 switch (request.action) {
 case DEBUGGER_ACTION_DUMP_TOMBSTONE:
 case DEBUGGER_ACTION_CRASH:
      tombstone_fd = open_tombstone(&tombstone_path);
 if (tombstone_fd == -1) {
        ALOGE("debuggerd: failed to open tombstone file: %s\n", strerror(errno));
        exit(1);
 }
 break;

 case DEBUGGER_ACTION_DUMP_BACKTRACE:
 break;

 default:
      ALOGE("debuggerd: unexpected request action: %d", request.action);
      exit(1);
 }


 
  if (!ptrace_attach_thread(request.pid, request.tid)) {
     ALOGE("debuggerd: ptrace attach failed: %s", strerror(errno));
     exit(1);
   }
 
  // DEBUGGER_ACTION_CRASH requests can come from arbitrary processes and the tid field in the
  // request is sent from the other side. If an attacker can cause a process to be spawned with the
  // pid of their process, they could trick debuggerd into dumping that process by exiting after
  // sending the request. Validate the trusted request.uid/gid to defend against this.
  if (request.action == DEBUGGER_ACTION_CRASH) {
    pid_t pid;
    uid_t uid;
    gid_t gid;
    if (get_process_info(request.tid, &pid, &uid, &gid) != 0) {
      ALOGE("debuggerd: failed to get process info for tid '%d'", request.tid);
      exit(1);
    }

    if (pid != request.pid || uid != request.uid || gid != request.gid) {
      ALOGE(
        "debuggerd: attached task %d does not match request: "
        "expected pid=%d,uid=%d,gid=%d, actual pid=%d,uid=%d,gid=%d",
        request.tid, request.pid, request.uid, request.gid, pid, uid, gid);
      exit(1);
    }
  }

   bool attach_gdb = should_attach_gdb(request);
 if (attach_gdb) {
 if (init_getevent() != 0) {
      ALOGE("debuggerd: failed to initialize input device, not waiting for gdb");
      attach_gdb = false;
 }

 }

  std::set<pid_t> siblings;
 if (!attach_gdb) {
    ptrace_siblings(request.pid, request.tid, siblings);
 }

  std::unique_ptr<BacktraceMap> backtrace_map(BacktraceMap::Create(request.pid));

 int amfd = -1;
  std::unique_ptr<std::string> amfd_data;
 if (request.action == DEBUGGER_ACTION_CRASH) {
    amfd = activity_manager_connect();
    amfd_data.reset(new std::string);
 }

 bool succeeded = false;

 if (!drop_privileges()) {
    ALOGE("debuggerd: failed to drop privileges, exiting");
    _exit(1);
 }

 int crash_signal = SIGKILL;
  succeeded = perform_dump(request, fd, tombstone_fd, backtrace_map.get(), siblings,
 &crash_signal, amfd_data.get());
 if (succeeded) {
 if (request.action == DEBUGGER_ACTION_DUMP_TOMBSTONE) {
 if (!tombstone_path.empty()) {
        android::base::WriteFully(fd, tombstone_path.c_str(), tombstone_path.length());
 }
 }
 }

 if (attach_gdb) {
 if (!send_signal(request.pid, 0, SIGSTOP)) {
      ALOGE("debuggerd: failed to stop process for gdb attach: %s", strerror(errno));
      attach_gdb = false;
 }
 }

 if (!attach_gdb) {
    activity_manager_write(request.pid, crash_signal, amfd, *amfd_data.get());
 }

 if (ptrace(PTRACE_DETACH, request.tid, 0, 0) != 0) {
    ALOGE("debuggerd: ptrace detach from %d failed: %s", request.tid, strerror(errno));
 }

 for (pid_t sibling : siblings) {
    ptrace(PTRACE_DETACH, sibling, 0, 0);
 }

 if (!attach_gdb && request.action == DEBUGGER_ACTION_CRASH) {
 if (!send_signal(request.pid, request.tid, crash_signal)) {
      ALOGE("debuggerd: failed to kill process %d: %s", request.pid, strerror(errno));
 }
 }

 if (attach_gdb) {
    wait_for_user_action(request);

    activity_manager_write(request.pid, crash_signal, amfd, *amfd_data.get());

 if (!send_signal(request.pid, 0, SIGCONT)) {
      ALOGE("debuggerd: failed to resume process %d: %s", request.pid, strerror(errno));
 }

    uninit_getevent();
 }

  close(amfd);

  exit(!succeeded);
}
