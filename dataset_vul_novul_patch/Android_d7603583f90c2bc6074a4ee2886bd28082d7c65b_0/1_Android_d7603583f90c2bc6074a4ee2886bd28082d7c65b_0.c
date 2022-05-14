 static int read_request(int fd, debugger_request_t* out_request) {
   ucred cr;
   socklen_t len = sizeof(cr);
 int status = getsockopt(fd, SOL_SOCKET, SO_PEERCRED, &cr, &len);
 if (status != 0) {
    ALOGE("cannot get credentials");
 return -1;
 }

  ALOGV("reading tid");
  fcntl(fd, F_SETFL, O_NONBLOCK);

  pollfd pollfds[1];
  pollfds[0].fd = fd;
  pollfds[0].events = POLLIN;
  pollfds[0].revents = 0;
  status = TEMP_FAILURE_RETRY(poll(pollfds, 1, 3000));
 if (status != 1) {
    ALOGE("timed out reading tid (from pid=%d uid=%d)\n", cr.pid, cr.uid);
 return -1;
 }

 debugger_msg_t msg;
  memset(&msg, 0, sizeof(msg));
  status = TEMP_FAILURE_RETRY(read(fd, &msg, sizeof(msg)));
 if (status < 0) {
    ALOGE("read failure? %s (pid=%d uid=%d)\n", strerror(errno), cr.pid, cr.uid);
 return -1;
 }
 if (status != sizeof(debugger_msg_t)) {
    ALOGE("invalid crash request of size %d (from pid=%d uid=%d)\n", status, cr.pid, cr.uid);
 return -1;
 }

  out_request->action = static_cast<debugger_action_t>(msg.action);
  out_request->tid = msg.tid;
  out_request->pid = cr.pid;
  out_request->uid = cr.uid;
  out_request->gid = cr.gid;
  out_request->abort_msg_address = msg.abort_msg_address;
  out_request->original_si_code = msg.original_si_code;

 
   if (msg.action == DEBUGGER_ACTION_CRASH) {
    char buf[64];
    struct stat s;
    snprintf(buf, sizeof buf, "/proc/%d/task/%d", out_request->pid, out_request->tid);
    if (stat(buf, &s)) {
      ALOGE("tid %d does not exist in pid %d. ignoring debug request\n",
          out_request->tid, out_request->pid);
       return -1;
     }
  } else if (cr.uid == 0
            || (cr.uid == AID_SYSTEM && msg.action == DEBUGGER_ACTION_DUMP_BACKTRACE)) {
     status = get_process_info(out_request->tid, &out_request->pid,
 &out_request->uid, &out_request->gid);
 if (status < 0) {
      ALOGE("tid %d does not exist. ignoring explicit dump request\n", out_request->tid);
 return -1;
 }

 if (!selinux_action_allowed(fd, out_request))
 return -1;
 } else {
 return -1;
 }
 return 0;
}
