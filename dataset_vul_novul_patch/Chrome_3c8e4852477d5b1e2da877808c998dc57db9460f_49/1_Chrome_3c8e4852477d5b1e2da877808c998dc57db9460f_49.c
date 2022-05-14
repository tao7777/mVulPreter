 void SharedWorkerDevToolsAgentHost::WorkerDestroyed() {
  DCHECK_NE(WORKER_TERMINATED, state_);
  DCHECK(worker_host_);
  state_ = WORKER_TERMINATED;
   for (auto* inspector : protocol::InspectorHandler::ForAgentHost(this))
     inspector->TargetCrashed();
   for (DevToolsSession* session : sessions())
    session->SetRenderer(nullptr, nullptr);
   worker_host_ = nullptr;
   agent_ptr_.reset();
 }
