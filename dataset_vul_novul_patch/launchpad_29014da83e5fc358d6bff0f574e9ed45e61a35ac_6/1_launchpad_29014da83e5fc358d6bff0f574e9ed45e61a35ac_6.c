 BrowserContextDestroyer::BrowserContextDestroyer(
    BrowserContext* context,
    const std::set<content::RenderProcessHost*>& hosts)
    : context_(context),
      pending_hosts_(0) {
  for (std::set<content::RenderProcessHost*>::iterator it = hosts.begin();
       it != hosts.end(); ++it) {
    (*it)->AddObserver(this);
    ++pending_hosts_;
   }
 }
