 std::string SyncerProtoUtil::SyncEntityDebugString(
     const sync_pb::SyncEntity& entry) {
   return base::StringPrintf(
       "id: %s, parent_id: %s, "
       "version: %"PRId64"d, "
      "mtime: %" PRId64"d (client: %" PRId64"d), "
      "ctime: %" PRId64"d (client: %" PRId64"d), "
       "name: %s, sync_timestamp: %" PRId64"d, "
       "%s ",
       entry.id_string().c_str(),
       entry.parent_id_string().c_str(),
       entry.version(),
      entry.mtime(), ServerTimeToClientTime(entry.mtime()),
      entry.ctime(), ServerTimeToClientTime(entry.ctime()),
       entry.name().c_str(), entry.sync_timestamp(),
       entry.deleted() ? "deleted, ":"");
 }
