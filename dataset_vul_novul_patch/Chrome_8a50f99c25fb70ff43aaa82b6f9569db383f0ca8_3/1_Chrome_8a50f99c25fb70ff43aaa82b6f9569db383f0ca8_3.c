void ChromeInvalidationClient::InvalidateAll(
     invalidation::InvalidationClient* client,
     const invalidation::AckHandle& ack_handle) {
   DCHECK(CalledOnValidThread());
   DVLOG(1) << "InvalidateAll";
 
   ObjectIdPayloadMap id_payloads;
  for (ObjectIdSet::const_iterator it = registered_ids_.begin();
       it != registered_ids_.end(); ++it) {
    id_payloads[*it] = std::string();
  }
  EmitInvalidation(id_payloads);
  client->Acknowledge(ack_handle);
}
