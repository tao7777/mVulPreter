 void WorkerProcessLauncher::Core::StopWorker() {
   DCHECK(caller_task_runner_->BelongsToCurrentThread());
 
   if (launch_success_timer_->IsRunning()) {
     launch_success_timer_->Stop();
     launch_backoff_.InformOfRequest(false);
   }
 
  self_ = this;
   ipc_enabled_ = false;
 
   if (process_watcher_.GetWatchedObject() != NULL) {
     launcher_delegate_->KillProcess(CONTROL_C_EXIT);
    return;
  }
 
  DCHECK(process_watcher_.GetWatchedObject() == NULL);
 
   ipc_error_timer_->Stop();
   process_exit_event_.Close();

   if (stopping_) {
     ipc_error_timer_.reset();
     launch_timer_.reset();
    self_ = NULL;
     return;
   }
 
  self_ = NULL;
  DWORD exit_code = launcher_delegate_->GetExitCode();
  if (kMinPermanentErrorExitCode <= exit_code &&
      exit_code <= kMaxPermanentErrorExitCode) {
    worker_delegate_->OnPermanentError();
    return;
   }
  launch_timer_->Start(FROM_HERE, launch_backoff_.GetTimeUntilRelease(),
                       this, &Core::LaunchWorker);
 }
