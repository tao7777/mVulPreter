bool AppCacheDatabase::FindCache(int64_t cache_id, CacheRecord* record) {
  DCHECK(record);
  if (!LazyOpen(kDontCreate))
     return false;
 
   static const char kSql[] =
      "SELECT cache_id, group_id, online_wildcard, update_time, cache_size"
       " FROM Caches WHERE cache_id = ?";
 
   sql::Statement statement(db_->GetCachedStatement(SQL_FROM_HERE, kSql));
  statement.BindInt64(0, cache_id);

  if (!statement.Step())
    return false;

  ReadCacheRecord(statement, record);
  return true;
}
