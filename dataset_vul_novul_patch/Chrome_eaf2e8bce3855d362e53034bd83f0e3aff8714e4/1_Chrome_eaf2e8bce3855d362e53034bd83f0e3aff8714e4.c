void IndexedDBDatabase::ForceClose() {
  scoped_refptr<IndexedDBDatabase> protect(this);

  while (!pending_requests_.empty()) {
    std::unique_ptr<ConnectionRequest> request =
        std::move(pending_requests_.front());
    pending_requests_.pop();
     request->AbortForForceClose();
   }
 
  auto it = connections_.begin();
  while (it != connections_.end()) {
    IndexedDBConnection* connection = *it++;
     connection->ForceClose();
   }
   DCHECK(connections_.empty());
   DCHECK(!active_request_);
}
