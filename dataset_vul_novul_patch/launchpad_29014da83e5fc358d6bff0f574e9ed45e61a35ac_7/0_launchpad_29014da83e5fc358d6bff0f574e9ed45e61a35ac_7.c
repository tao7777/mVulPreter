void BrowserContextDestroyer::DestroyContext(BrowserContext* context) {
void BrowserContextDestroyer::Shutdown() {
  auto destroy_all_unused_contexts = []() {
    auto it = g_contexts_pending_deletion.Get().begin();
    while (it != g_contexts_pending_deletion.Get().end()) {
      BrowserContextDestroyer* destroyer = *it;
      ++it;
 
      if (!destroyer->finish_destroy_scheduled_) {
        continue;
      }
 
      destroyer->FinishDestroyContext();
      // |destroyer| is invalid now
     }
  };
 
  // We make 2 passes over the list because the first pass can destroy an
  // incognito BrowserContext that subsequently schedules its owner context for
  // deletion
  destroy_all_unused_contexts();
  destroy_all_unused_contexts();
}

//// static
void BrowserContextDestroyer::RenderProcessHostAssignedToSiteInstance(
    content::RenderProcessHost* host) {
  BrowserContextDestroyer* destroyer = GetForContext(host->GetBrowserContext());
  if (!destroyer) {
    return;
   }
 
  CHECK(!destroyer->finish_destroy_scheduled_);
 
  if (destroyer->pending_host_ids_.find(host->GetID()) !=
      destroyer->pending_host_ids_.end()) {
    return;
   }

  destroyer->ObserveHost(host);
 }
