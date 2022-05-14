bool SessionManager::Remove(const std::string& id) {
  std::map<std::string, Session*>::iterator it;
   Session* session;
   base::AutoLock lock(map_lock_);
   it = map_.find(id);
  if (it == map_.end())
     return false;
   session = it->second;
   map_.erase(it);
   return true;
}
