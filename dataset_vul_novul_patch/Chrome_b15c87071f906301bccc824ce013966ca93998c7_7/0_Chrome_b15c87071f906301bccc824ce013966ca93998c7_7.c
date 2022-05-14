 void WorkerProcessLauncher::Core::OnChannelConnected(int32 peer_pid) {
   DCHECK(caller_task_runner_->BelongsToCurrentThread());
 
  if (!ipc_enabled_)
    return;

  // Verify |peer_pid| because it is controlled by the client and cannot be
  // trusted.
  DWORD actual_pid = launcher_delegate_->GetProcessId();
  if (peer_pid != static_cast<int32>(actual_pid)) {
    LOG(ERROR) << "The actual client PID " << actual_pid
               << " does not match the one reported by the client: "
               << peer_pid;
    StopWorker();
    return;
  }

  worker_delegate_->OnChannelConnected(peer_pid);
 }
