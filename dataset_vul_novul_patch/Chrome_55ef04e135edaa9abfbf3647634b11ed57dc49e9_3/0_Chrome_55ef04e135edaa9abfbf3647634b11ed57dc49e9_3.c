 Resource::~Resource() {
  ResourceTracker::Get()->ResourceDestroyed(this);
 }
