const base::Time& BaseNode::GetModificationTime() const {
   return GetEntry()->Get(syncable::MTIME);
 }
