void ChromeInvalidationClient::RegisterIds(const ObjectIdSet& ids) {
void ChromeInvalidationClient::UpdateRegisteredIds(const ObjectIdSet& ids) {
   DCHECK(CalledOnValidThread());
   registered_ids_ = ids;
   if (ticl_state_ == NO_NOTIFICATION_ERROR && registration_manager_.get()) {
    registration_manager_->UpdateRegisteredIds(registered_ids_);
   }
 }
