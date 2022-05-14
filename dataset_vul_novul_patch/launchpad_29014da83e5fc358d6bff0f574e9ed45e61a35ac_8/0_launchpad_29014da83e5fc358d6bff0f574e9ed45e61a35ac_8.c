 void BrowserContextDestroyer::FinishDestroyContext() {
  DCHECK(finish_destroy_scheduled_);
  CHECK_EQ(GetHostsForContext(context_.get()).size(), 0U)
      << "One or more RenderProcessHosts exist whilst its BrowserContext is "
      << "being deleted!";
 
  g_contexts_pending_deletion.Get().remove(this);

  if (context_->IsOffTheRecord()) {
    // If this is an OTR context and its owner BrowserContext has been scheduled
    // for deletion, update the owner's BrowserContextDestroyer
    BrowserContextDestroyer* orig_destroyer =
        GetForContext(context_->GetOriginalContext());
    if (orig_destroyer) {
      DCHECK_GT(orig_destroyer->otr_contexts_pending_deletion_, 0U);
      DCHECK(!orig_destroyer->finish_destroy_scheduled_);
      --orig_destroyer->otr_contexts_pending_deletion_;
      orig_destroyer->MaybeScheduleFinishDestroyContext();
    }
  }
 
   delete this;
 }
