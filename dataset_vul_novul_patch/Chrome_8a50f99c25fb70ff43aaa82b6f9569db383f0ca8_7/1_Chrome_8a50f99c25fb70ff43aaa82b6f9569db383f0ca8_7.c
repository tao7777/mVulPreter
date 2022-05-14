void ChromeInvalidationClient::ReissueRegistrations(
    invalidation::InvalidationClient* client,
     const std::string& prefix,
     int prefix_length) {
   DCHECK(CalledOnValidThread());
   DVLOG(1) << "AllRegistrationsLost";
   registration_manager_->MarkAllRegistrationsLost();
 }
