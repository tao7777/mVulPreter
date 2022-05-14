 void ServiceWorkerDevToolsAgentHost::WorkerDestroyed() {
  DCHECK_NE(WORKER_TERMINATED, state_);
  state_ = WORKER_TERMINATED;
  agent_ptr_.reset();
   for (auto* inspector : protocol::InspectorHandler::ForAgentHost(this))
     inspector->TargetCrashed();
   for (DevToolsSession* session : sessions())
    session->SetRenderer(-1, nullptr);
 }
