void ServiceWorkerDevToolsAgentHost::WorkerReadyForInspection(
    blink::mojom::DevToolsAgentAssociatedPtrInfo devtools_agent_ptr_info) {
  DCHECK_EQ(WORKER_NOT_READY, state_);
  state_ = WORKER_READY;
  agent_ptr_.Bind(std::move(devtools_agent_ptr_info));
  if (!sessions().empty()) {
    BrowserThread::PostTask(BrowserThread::IO, FROM_HERE,
                            base::BindOnce(&SetDevToolsAttachedOnIO,
                                            context_weak_, version_id_, true));
   }
 
  RenderProcessHost* host = RenderProcessHost::FromID(worker_process_id_);
   for (DevToolsSession* session : sessions()) {
    session->SetRenderer(host, nullptr);
     session->AttachToAgent(agent_ptr_);
   }
 }
