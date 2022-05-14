void ChromeInvalidationClient::InformRegistrationFailure(
    invalidation::InvalidationClient* client,
    const invalidation::ObjectId& object_id,
     bool is_transient,
     const std::string& error_message) {
   DCHECK(CalledOnValidThread());
   DVLOG(1) << "InformRegistrationFailure: "
            << ObjectIdToString(object_id)
            << "is_transient=" << is_transient
           << ", message=" << error_message;

  if (is_transient) {
    registration_manager_->MarkRegistrationLost(object_id);
  } else {
    registration_manager_->DisableId(object_id);
  }
}
