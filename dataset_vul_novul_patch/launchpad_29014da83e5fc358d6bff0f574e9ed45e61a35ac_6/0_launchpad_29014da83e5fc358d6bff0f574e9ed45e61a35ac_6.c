 BrowserContextDestroyer::BrowserContextDestroyer(
    std::unique_ptr<BrowserContext> context,
    const std::set<content::RenderProcessHost*>& hosts,
    uint32_t otr_contexts_pending_deletion)
    : context_(std::move(context)),
      otr_contexts_pending_deletion_(otr_contexts_pending_deletion),
      finish_destroy_scheduled_(false) {
  DCHECK(hosts.size() > 0 ||
         (!context->IsOffTheRecord() &&
          (otr_contexts_pending_deletion > 0 ||
               context->HasOffTheRecordContext())));

  g_contexts_pending_deletion.Get().push_back(this);

  for (auto* host : hosts) {
    ObserveHost(host);
   }
 }
