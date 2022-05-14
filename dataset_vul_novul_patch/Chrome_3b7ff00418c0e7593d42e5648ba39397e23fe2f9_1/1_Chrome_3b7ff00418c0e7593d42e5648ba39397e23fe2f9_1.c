 void SyncManager::SyncInternal::OnIPAddressChangedImpl() {
   DCHECK(thread_checker_.CalledOnValidThread());
  connection_manager()->CheckServerReachable();
 }
