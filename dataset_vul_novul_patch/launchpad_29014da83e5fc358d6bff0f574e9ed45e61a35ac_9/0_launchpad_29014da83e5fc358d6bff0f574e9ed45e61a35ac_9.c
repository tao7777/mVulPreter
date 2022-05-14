 void BrowserContextDestroyer::RenderProcessHostDestroyed(
     content::RenderProcessHost* host) {
  DCHECK_GT(pending_host_ids_.size(), 0U);

  size_t erased = pending_host_ids_.erase(host->GetID());
  DCHECK_GT(erased, 0U);

  MaybeScheduleFinishDestroyContext(host);
}

//// static
void BrowserContextDestroyer::DestroyContext(
    std::unique_ptr<BrowserContext> context) {
 
  bool has_live_otr_context = false;
  uint32_t otr_contexts_pending_deletion = 0;

  if (!context->IsOffTheRecord()) {
    // If |context| is not an OTR BrowserContext, we need to keep track of how
    // many OTR BrowserContexts that were owned by it are scheduled for deletion
    // but still exist, as |context| must outlive these
    for (auto* destroyer : g_contexts_pending_deletion.Get()) {
      if (destroyer->context_->IsOffTheRecord() &&
          destroyer->context_->GetOriginalContext() == context.get()) {
        ++otr_contexts_pending_deletion;
      }
    }

    // If |context| is not an OTR BrowserContext but currently owns a live OTR
    // BrowserContext, then we have to outlive that
    has_live_otr_context = context->HasOffTheRecordContext();
   } else {
    // If |context| is an OTR BrowserContext and its owner has already been
    // scheduled for deletion, then we need to prevent the owner from being
    // deleted until after |context|
    BrowserContextDestroyer* orig_destroyer =
        GetForContext(context->GetOriginalContext());
    if (orig_destroyer) {
      CHECK(!orig_destroyer->finish_destroy_scheduled_);
      ++orig_destroyer->otr_contexts_pending_deletion_;
    }
  }

  // Get all of the live RenderProcessHosts that are using |context|
  std::set<content::RenderProcessHost*> hosts =
      GetHostsForContext(context.get());

  content::BrowserContext::NotifyWillBeDestroyed(context.get());

  // |hosts| might not be empty if the application released its BrowserContext
  // too early, or if |context| is an OTR context or this application is single
  // process

  if (!hosts.empty() ||
      otr_contexts_pending_deletion > 0 ||
      has_live_otr_context) {
    // |context| is not safe to delete yet
    new BrowserContextDestroyer(std::move(context),
                                hosts,
                                otr_contexts_pending_deletion);
   }
 }
