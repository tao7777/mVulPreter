Session* SessionManager::GetSession(const std::string& id) const {
   std::map<std::string, Session*>::const_iterator it;
   base::AutoLock lock(map_lock_);
   it = map_.find(id);
  if (it == map_.end()) {
    VLOG(1) << "No such session with ID " << id;
     return NULL;
  }
   return it->second;
 }
