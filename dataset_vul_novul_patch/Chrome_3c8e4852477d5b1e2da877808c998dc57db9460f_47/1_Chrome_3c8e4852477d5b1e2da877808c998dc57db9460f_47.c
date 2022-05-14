 BrowserContext* SharedWorkerDevToolsAgentHost::GetBrowserContext() {
  RenderProcessHost* rph = GetProcess();
   return rph ? rph->GetBrowserContext() : nullptr;
 }
