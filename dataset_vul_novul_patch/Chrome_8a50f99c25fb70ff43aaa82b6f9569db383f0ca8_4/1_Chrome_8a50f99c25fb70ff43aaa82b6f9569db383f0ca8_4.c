void ChromeInvalidationClient::InvalidateUnknownVersion(
    invalidation::InvalidationClient* client,
     const invalidation::ObjectId& object_id,
     const invalidation::AckHandle& ack_handle) {
   DCHECK(CalledOnValidThread());
   DVLOG(1) << "InvalidateUnknownVersion";
 
   ObjectIdPayloadMap id_payloads;
  id_payloads[object_id] = std::string();
  EmitInvalidation(id_payloads);
  client->Acknowledge(ack_handle);
}
