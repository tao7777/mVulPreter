RenderProcessHost* SharedWorkerDevToolsAgentHost::GetProcess() {
  return worker_host_ ? RenderProcessHost::FromID(worker_host_->process_id())
                      : nullptr;
}
