void SyncManager::MaybeSetSyncTabsInNigoriNode(
    ModelTypeSet enabled_types) {
  DCHECK(thread_checker_.CalledOnValidThread());
  data_->MaybeSetSyncTabsInNigoriNode(enabled_types);
}
