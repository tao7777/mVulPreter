  void Set(const std::string& addr, int value) {
  // Sets the |value| for |preview_id|.
  void Set(int32 preview_id, int value) {
     base::AutoLock lock(lock_);
    map_[preview_id] = value;
   }
