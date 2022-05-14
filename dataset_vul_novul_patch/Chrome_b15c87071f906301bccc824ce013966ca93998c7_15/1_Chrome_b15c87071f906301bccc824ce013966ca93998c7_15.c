DWORD WtsSessionProcessDelegate::Core::GetExitCode() {
  DCHECK(main_task_runner_->BelongsToCurrentThread());
 
   DWORD exit_code = CONTROL_C_EXIT;
   if (worker_process_.IsValid()) {
     if (!::GetExitCodeProcess(worker_process_, &exit_code)) {
      LOG_GETLASTERROR(INFO)
          << "Failed to query the exit code of the worker process";
      exit_code = CONTROL_C_EXIT;
     }
   }
 
  return exit_code;
 }
