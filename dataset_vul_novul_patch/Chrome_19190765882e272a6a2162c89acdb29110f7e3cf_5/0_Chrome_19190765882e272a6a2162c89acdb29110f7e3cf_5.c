 bool SyncerUtil::ServerAndLocalEntriesMatch(syncable::Entry* entry) {
  if (!ClientAndServerTimeMatch(
        entry->Get(CTIME), ClientTimeToServerTime(entry->Get(SERVER_CTIME)))) {
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
  bool time_match = ClientAndServerTimeMatch(entry->Get(MTIME),
      ClientTimeToServerTime(entry->Get(SERVER_MTIME)));
  if (!time_match) {
     LOG(WARNING) << "Time mismatch";
   }
  return time_match;
 }
