bool SessionManager::Remove(const std::string& id) {
  std::map<std::string, Session*>::iterator it;
   Session* session;
   base::AutoLock lock(map_lock_);
   it = map_.find(id);
  if (it == map_.end()) {
    VLOG(1) << "No such session with ID " << id;
     return false;
  }
   session = it->second;
   map_.erase(it);
   return true;
}
