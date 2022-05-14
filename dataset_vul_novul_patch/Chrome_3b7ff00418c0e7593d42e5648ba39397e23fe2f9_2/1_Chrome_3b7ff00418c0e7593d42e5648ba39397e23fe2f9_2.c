void SyncManager::SyncInternal::UpdateCredentials(
    const SyncCredentials& credentials) {
  DCHECK(thread_checker_.CalledOnValidThread());
  DCHECK_EQ(credentials.email, share_.name);
  DCHECK(!credentials.email.empty());
  DCHECK(!credentials.sync_token.empty());

  observing_ip_address_changes_ = true;
  if (connection_manager()->set_auth_token(credentials.sync_token)) {
     sync_notifier_->UpdateCredentials(
         credentials.email, credentials.sync_token);
     if (!setup_for_test_mode_) {
      CheckServerReachable();
     }
   }
 }
