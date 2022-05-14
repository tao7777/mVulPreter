 std::string SyncerProtoUtil::SyncEntityDebugString(
     const sync_pb::SyncEntity& entry) {
  const std::string& mtime_str =
      GetTimeDebugString(ProtoTimeToTime(entry.mtime()));
  const std::string& ctime_str =
      GetTimeDebugString(ProtoTimeToTime(entry.ctime()));
   return base::StringPrintf(
       "id: %s, parent_id: %s, "
       "version: %"PRId64"d, "
      "mtime: %" PRId64"d (%s), "
      "ctime: %" PRId64"d (%s), "
       "name: %s, sync_timestamp: %" PRId64"d, "
       "%s ",
       entry.id_string().c_str(),
       entry.parent_id_string().c_str(),
       entry.version(),
      entry.mtime(), mtime_str.c_str(),
      entry.ctime(), ctime_str.c_str(),
       entry.name().c_str(), entry.sync_timestamp(),
       entry.deleted() ? "deleted, ":"");
 }
