 void ChromeInvalidationClient::Ready(
     invalidation::InvalidationClient* client) {
   ticl_state_ = NO_NOTIFICATION_ERROR;
   EmitStateChange();
  registration_manager_->SetRegisteredIds(registered_ids_);
 }
