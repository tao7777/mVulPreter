void IndexedDBDatabase::ForceClose() {
  scoped_refptr<IndexedDBDatabase> protect(this);

  while (!pending_requests_.empty()) {
    std::unique_ptr<ConnectionRequest> request =
        std::move(pending_requests_.front());
    pending_requests_.pop();
     request->AbortForForceClose();
   }
 
  while (!connections_.empty()) {
    IndexedDBConnection* connection = *connections_.begin();
     connection->ForceClose();
    connections_.erase(connection);
   }
   DCHECK(connections_.empty());
   DCHECK(!active_request_);
}
