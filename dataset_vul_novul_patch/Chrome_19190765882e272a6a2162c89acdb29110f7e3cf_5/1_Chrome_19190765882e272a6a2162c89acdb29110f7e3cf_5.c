 bool SyncerUtil::ServerAndLocalEntriesMatch(syncable::Entry* entry) {
  if (entry->Get(CTIME) != entry->Get(SERVER_CTIME)) {
     LOG(WARNING) << "Client and server time mismatch";
     return false;
   }
  if (entry->Get(IS_DEL) && entry->Get(SERVER_IS_DEL))
    return true;
  if (!(entry->Get(NON_UNIQUE_NAME) == entry->Get(SERVER_NON_UNIQUE_NAME))) {
    LOG(WARNING) << "Unsanitized name mismatch";
    return false;
  }

  if (entry->Get(PARENT_ID) != entry->Get(SERVER_PARENT_ID) ||
      entry->Get(IS_DIR) != entry->Get(SERVER_IS_DIR) ||
      entry->Get(IS_DEL) != entry->Get(SERVER_IS_DEL)) {
    LOG(WARNING) << "Metabit mismatch";
    return false;
  }

  if (!ServerAndLocalOrdersMatch(entry)) {
    LOG(WARNING) << "Server/local ordering mismatch";
    return false;
  }

  if (entry->Get(SPECIFICS).SerializeAsString() !=
      entry->Get(SERVER_SPECIFICS).SerializeAsString()) {
    LOG(WARNING) << "Specifics mismatch";
    return false;
  }
  if (entry->Get(IS_DIR))
     return true;
  if (entry->Get(MTIME) != entry->Get(SERVER_MTIME)) {
     LOG(WARNING) << "Time mismatch";
    return false;
   }
  return true;
 }
