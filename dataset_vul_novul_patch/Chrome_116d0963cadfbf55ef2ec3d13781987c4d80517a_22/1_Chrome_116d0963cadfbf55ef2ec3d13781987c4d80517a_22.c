  void Set(const std::string& addr, int value) {
     base::AutoLock lock(lock_);
    map_[addr] = value;
   }
