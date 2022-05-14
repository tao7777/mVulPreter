  void Erase(const std::string& addr) {
     base::AutoLock lock(lock_);
    map_.erase(addr);
   }
