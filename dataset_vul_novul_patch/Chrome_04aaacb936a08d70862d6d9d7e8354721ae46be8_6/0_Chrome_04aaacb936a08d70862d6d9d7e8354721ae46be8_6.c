bool AppCacheDatabase::FindCacheForGroup(int64_t group_id,
                                         CacheRecord* record) {
  DCHECK(record);
  if (!LazyOpen(kDontCreate))
     return false;
 
   static const char kSql[] =
      "SELECT cache_id, group_id, online_wildcard, update_time, cache_size, "
      "padding_size"
       "  FROM Caches WHERE group_id = ?";
 
   sql::Statement statement(db_->GetCachedStatement(SQL_FROM_HERE, kSql));
  statement.BindInt64(0, group_id);

  if (!statement.Step())
    return false;

  ReadCacheRecord(statement, record);
  return true;
}
