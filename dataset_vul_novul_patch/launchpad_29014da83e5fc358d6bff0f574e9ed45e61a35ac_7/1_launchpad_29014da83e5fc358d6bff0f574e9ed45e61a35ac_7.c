void BrowserContextDestroyer::DestroyContext(BrowserContext* context) {
  CHECK(context->IsOffTheRecord() || !context->HasOffTheRecordContext());
 
  content::BrowserContext::NotifyWillBeDestroyed(context);
  std::set<content::RenderProcessHost*> hosts;
 
  for (content::RenderProcessHost::iterator it =
           content::RenderProcessHost::AllHostsIterator();
       !it.IsAtEnd(); it.Advance()) {
    content::RenderProcessHost* host = it.GetCurrentValue();
    if (host->GetBrowserContext() != context) {
      continue;
     }
 
    hosts.insert(host);
//// static
   }
 
 
  if (hosts.empty()) {
    delete context;
  } else {
    new BrowserContextDestroyer(context, hosts);
   }
 }
