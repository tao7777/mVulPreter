void SharedWorkerDevToolsAgentHost::WorkerRestarted(
    SharedWorkerHost* worker_host) {
  DCHECK_EQ(WORKER_TERMINATED, state_);
  DCHECK(!worker_host_);
   state_ = WORKER_NOT_READY;
   worker_host_ = worker_host;
   for (DevToolsSession* session : sessions())
    session->SetRenderer(GetProcess(), nullptr);
 }
