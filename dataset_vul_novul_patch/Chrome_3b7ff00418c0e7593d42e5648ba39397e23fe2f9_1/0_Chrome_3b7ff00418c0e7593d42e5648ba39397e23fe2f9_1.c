 void SyncManager::SyncInternal::OnIPAddressChangedImpl() {
   DCHECK(thread_checker_.CalledOnValidThread());
  CheckServerReachable();
 }
