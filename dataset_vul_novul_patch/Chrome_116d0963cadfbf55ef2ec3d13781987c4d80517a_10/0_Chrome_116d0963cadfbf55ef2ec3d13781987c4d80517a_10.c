  void Erase(const std::string& addr) {
  // Erases the entry for |preview_id|.
  void Erase(int32 preview_id) {
     base::AutoLock lock(lock_);
    map_.erase(preview_id);
   }
