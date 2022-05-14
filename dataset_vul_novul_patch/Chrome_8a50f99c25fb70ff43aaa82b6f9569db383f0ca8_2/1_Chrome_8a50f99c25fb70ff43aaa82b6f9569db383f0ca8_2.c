 void ChromeInvalidationClient::Invalidate(
     invalidation::InvalidationClient* client,
     const invalidation::Invalidation& invalidation,
     const invalidation::AckHandle& ack_handle) {
   DCHECK(CalledOnValidThread());
   DVLOG(1) << "Invalidate: " << InvalidationToString(invalidation);
 
   const invalidation::ObjectId& id = invalidation.object_id();

  InvalidationVersionMap::const_iterator it =
      max_invalidation_versions_.find(id);
  if ((it != max_invalidation_versions_.end()) &&
      (invalidation.version() <= it->second)) {
    client->Acknowledge(ack_handle);
    return;
  }
  DVLOG(2) << "Setting max invalidation version for " << ObjectIdToString(id)
           << " to " << invalidation.version();
  max_invalidation_versions_[id] = invalidation.version();
  invalidation_state_tracker_.Call(
      FROM_HERE,
      &InvalidationStateTracker::SetMaxVersion,
      id, invalidation.version());

  std::string payload;
  if (invalidation.has_payload())
    payload = invalidation.payload();

  ObjectIdPayloadMap id_payloads;
  id_payloads[id] = payload;
  EmitInvalidation(id_payloads);
  client->Acknowledge(ack_handle);
}
