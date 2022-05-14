void SyncManager::SyncInternal::OnIPAddressChanged() {
  DVLOG(1) << "IP address change detected";
  if (!observing_ip_address_changes_) {
    DVLOG(1) << "IP address change dropped.";
     return;
   }
 
   OnIPAddressChangedImpl();
 }
