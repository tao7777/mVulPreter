 void WtsSessionProcessDelegate::Core::KillProcess(DWORD exit_code) {
   DCHECK(main_task_runner_->BelongsToCurrentThread());
 
   channel_.reset();
 
   if (launch_elevated_) {
     if (job_.IsValid()) {
      TerminateJobObject(job_, exit_code);
    }
  } else {
    if (worker_process_.IsValid()) {
      TerminateProcess(worker_process_, exit_code);
    }
  }
}
