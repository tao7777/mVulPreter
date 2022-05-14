bool DirectoryBackingStore::MigrateVersion76To77() {
  sqlite_utils::SQLStatement update_timestamps;
#if defined(OS_WIN)
#define TO_UNIX_TIME_MS(x) #x " = " #x " / 10000 - 11644473600000"
#else
#define TO_UNIX_TIME_MS(x) #x " = " #x " * 1000"
#endif
  update_timestamps.prepare(
      load_dbhandle_,
      "UPDATE metas SET "
      TO_UNIX_TIME_MS(mtime) ", "
      TO_UNIX_TIME_MS(server_mtime) ", "
      TO_UNIX_TIME_MS(ctime) ", "
      TO_UNIX_TIME_MS(server_ctime));
#undef TO_UNIX_TIME_MS
  if (update_timestamps.step() != SQLITE_DONE)
    return false;
  SetVersion(77);
  return true;
}
