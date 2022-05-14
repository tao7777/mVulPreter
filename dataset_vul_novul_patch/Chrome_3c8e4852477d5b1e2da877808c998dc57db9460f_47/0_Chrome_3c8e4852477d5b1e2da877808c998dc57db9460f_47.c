 BrowserContext* SharedWorkerDevToolsAgentHost::GetBrowserContext() {
  if (!worker_host_)
    return nullptr;
  RenderProcessHost* rph =
      RenderProcessHost::FromID(worker_host_->process_id());
   return rph ? rph->GetBrowserContext() : nullptr;
 }
