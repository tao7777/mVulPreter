void ServiceWorkerDevToolsAgentHost::WorkerRestarted(int worker_process_id,
                                                     int worker_route_id) {
  DCHECK_EQ(WORKER_TERMINATED, state_);
   state_ = WORKER_NOT_READY;
   worker_process_id_ = worker_process_id;
   worker_route_id_ = worker_route_id;
  RenderProcessHost* host = RenderProcessHost::FromID(worker_process_id_);
   for (DevToolsSession* session : sessions())
    session->SetRenderer(host, nullptr);
 }
