Session* SessionManager::GetSession(const std::string& id) const {
   std::map<std::string, Session*>::const_iterator it;
   base::AutoLock lock(map_lock_);
   it = map_.find(id);
  if (it == map_.end())
     return NULL;
   return it->second;
 }
