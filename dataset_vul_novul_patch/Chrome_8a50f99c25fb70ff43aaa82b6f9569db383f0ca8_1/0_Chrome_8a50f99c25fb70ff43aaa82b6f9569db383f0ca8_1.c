void ChromeInvalidationClient::InformRegistrationStatus(
      invalidation::InvalidationClient* client,
       const invalidation::ObjectId& object_id,
       InvalidationListener::RegistrationState new_state) {
   DCHECK(CalledOnValidThread());
  DCHECK_EQ(client, invalidation_client_.get());
   DVLOG(1) << "InformRegistrationStatus: "
            << ObjectIdToString(object_id) << " " << new_state;
 
  if (new_state != InvalidationListener::REGISTERED) {
    registration_manager_->MarkRegistrationLost(object_id);
  }
}
