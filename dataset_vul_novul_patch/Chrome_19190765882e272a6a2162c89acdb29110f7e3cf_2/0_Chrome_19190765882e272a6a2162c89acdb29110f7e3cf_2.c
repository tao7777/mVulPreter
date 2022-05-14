bool SyncerProtoUtil::Compare(const syncable::Entry& local_entry,
                              const SyncEntity& server_entry) {
  const std::string name = NameFromSyncEntity(server_entry);

  CHECK(local_entry.Get(ID) == server_entry.id()) <<
      " SyncerProtoUtil::Compare precondition not met.";
  CHECK(server_entry.version() == local_entry.Get(BASE_VERSION)) <<
      " SyncerProtoUtil::Compare precondition not met.";
  CHECK(!local_entry.Get(IS_UNSYNCED)) <<
      " SyncerProtoUtil::Compare precondition not met.";
 
   if (local_entry.Get(IS_DEL) && server_entry.deleted())
     return true;
  if (!ClientAndServerTimeMatch(local_entry.Get(CTIME), server_entry.ctime())) {
     LOG(WARNING) << "ctime mismatch";
     return false;
   }

  string client_name = local_entry.Get(syncable::NON_UNIQUE_NAME);
  if (client_name != name) {
    LOG(WARNING) << "Client name mismatch";
    return false;
  }
  if (local_entry.Get(PARENT_ID) != server_entry.parent_id()) {
    LOG(WARNING) << "Parent ID mismatch";
    return false;
  }
  if (local_entry.Get(IS_DIR) != server_entry.IsFolder()) {
    LOG(WARNING) << "Dir field mismatch";
    return false;
  }
  if (local_entry.Get(IS_DEL) != server_entry.deleted()) {
    LOG(WARNING) << "Deletion mismatch";
     return false;
   }
   if (!local_entry.Get(IS_DIR) &&
      !ClientAndServerTimeMatch(local_entry.Get(MTIME),
                                server_entry.mtime())) {
     LOG(WARNING) << "mtime mismatch";
     return false;
   }

  return true;
}
