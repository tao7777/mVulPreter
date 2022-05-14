void ChromeInvalidationClient::ReissueRegistrations(
    invalidation::InvalidationClient* client,
     const std::string& prefix,
     int prefix_length) {
   DCHECK(CalledOnValidThread());
  DCHECK_EQ(client, invalidation_client_.get());
   DVLOG(1) << "AllRegistrationsLost";
   registration_manager_->MarkAllRegistrationsLost();
 }
