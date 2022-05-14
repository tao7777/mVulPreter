static void ptrace_siblings(pid_t pid, pid_t main_tid, std::set<pid_t>& tids) {
  char task_path[64];
//// Attach to a thread, and verify that it's still a member of the given process
 
  snprintf(task_path, sizeof(task_path), "/proc/%d/task", pid);
 
   std::unique_ptr<DIR, int (*)(DIR*)> d(opendir(task_path), closedir);
 
 if (!d) {
    ALOGE("debuggerd: failed to open /proc/%d/task: %s", pid, strerror(errno));
 return;
 }

 struct dirent* de;
 while ((de = readdir(d.get())) != NULL) {
 if (!strcmp(de->d_name, ".") || !strcmp(de->d_name, "..")) {
 continue;
 }

 char* end;
 pid_t tid = strtoul(de->d_name, &end, 10);
 if (*end) {
 continue;
 }

 if (tid == main_tid) {

       continue;
     }
 
    if (ptrace(PTRACE_ATTACH, tid, 0, 0) < 0) {
       ALOGE("debuggerd: ptrace attach to %d failed: %s", tid, strerror(errno));
       continue;
     }

    tids.insert(tid);
 }
}
