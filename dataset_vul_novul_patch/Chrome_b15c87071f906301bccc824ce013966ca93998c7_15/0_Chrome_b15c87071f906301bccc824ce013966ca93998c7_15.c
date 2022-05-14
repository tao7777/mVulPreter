DWORD WtsSessionProcessDelegate::Core::GetExitCode() {
DWORD WtsSessionProcessDelegate::Core::GetProcessId() const {
  DWORD pid = 0;
  if (pipe_.IsValid() && get_named_pipe_client_pid_(pipe_, &pid)) {
    return pid;
  } else {
    return 0;
  }
}
 
bool WtsSessionProcessDelegate::Core::IsPermanentError(
    int failure_count) const {
  // Get exit code of the worker process if it is available.
   DWORD exit_code = CONTROL_C_EXIT;
   if (worker_process_.IsValid()) {
     if (!::GetExitCodeProcess(worker_process_, &exit_code)) {
      LOG_GETLASTERROR(INFO)
          << "Failed to query the exit code of the worker process";
      exit_code = CONTROL_C_EXIT;
     }
   }
 
  // Stop trying to restart the worker process if it exited due to
  // misconfiguration.
  return (kMinPermanentErrorExitCode <= exit_code &&
      exit_code <= kMaxPermanentErrorExitCode);
 }
