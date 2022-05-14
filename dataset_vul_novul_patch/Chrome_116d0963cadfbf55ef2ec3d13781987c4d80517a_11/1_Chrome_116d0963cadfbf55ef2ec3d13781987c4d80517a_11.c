  bool Get(const std::string& addr, int* out_value) {
     base::AutoLock lock(lock_);
    PrintPreviewRequestIdMap::const_iterator it = map_.find(addr);
     if (it == map_.end())
       return false;
     *out_value = it->second;
     return true;
   }
