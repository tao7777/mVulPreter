const base::Time& BaseNode::GetModificationTime() const {
int64 BaseNode::GetModificationTime() const {
   return GetEntry()->Get(syncable::MTIME);
 }
