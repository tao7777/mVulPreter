 void WorkerProcessLauncher::Core::StopWorker() {
   DCHECK(caller_task_runner_->BelongsToCurrentThread());
 
  // Keep the object alive in case one of delegates decides to delete |this|.
  scoped_refptr<Core> self = this;

   if (launch_success_timer_->IsRunning()) {
     launch_success_timer_->Stop();
     launch_backoff_.InformOfRequest(false);
   }
 
   ipc_enabled_ = false;
 
   if (process_watcher_.GetWatchedObject() != NULL) {
     launcher_delegate_->KillProcess(CONTROL_C_EXIT);
 
    // Wait until the process is actually stopped if the caller keeps
    // a reference to |this|. Otherwise terminate everything right now - there
    // won't be a second chance.
    if (!stopping_)
      return;

    process_watcher_.StopWatching();
  }
 
   ipc_error_timer_->Stop();
   process_exit_event_.Close();

   if (stopping_) {
     ipc_error_timer_.reset();
     launch_timer_.reset();
     return;
   }
 
  if (launcher_delegate_->IsPermanentError(launch_backoff_.failure_count())) {
    if (!stopping_)
      worker_delegate_->OnPermanentError();
  } else {
    // Schedule the next attempt to launch the worker process.
    launch_timer_->Start(FROM_HERE, launch_backoff_.GetTimeUntilRelease(),
                         this, &Core::LaunchWorker);
   }
 }
