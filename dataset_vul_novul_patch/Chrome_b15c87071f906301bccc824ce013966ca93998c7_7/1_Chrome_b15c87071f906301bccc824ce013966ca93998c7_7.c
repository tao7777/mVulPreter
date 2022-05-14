 void WorkerProcessLauncher::Core::OnChannelConnected(int32 peer_pid) {
   DCHECK(caller_task_runner_->BelongsToCurrentThread());
 
  if (ipc_enabled_)
    worker_delegate_->OnChannelConnected();
 }
