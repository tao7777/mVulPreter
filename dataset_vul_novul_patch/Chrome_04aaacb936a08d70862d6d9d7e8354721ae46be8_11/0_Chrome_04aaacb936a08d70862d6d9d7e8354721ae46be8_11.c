bool AppCacheDatabase::InsertCache(const CacheRecord* record) {
  if (!LazyOpen(kCreateIfNeeded))
    return false;
 
   static const char kSql[] =
       "INSERT INTO Caches (cache_id, group_id, online_wildcard,"
      "                    update_time, cache_size, padding_size)"
      "  VALUES(?, ?, ?, ?, ?, ?)";
 
   sql::Statement statement(db_->GetCachedStatement(SQL_FROM_HERE, kSql));
   statement.BindInt64(0, record->cache_id);
   statement.BindInt64(1, record->group_id);
   statement.BindBool(2, record->online_wildcard);
   statement.BindInt64(3, record->update_time.ToInternalValue());
  DCHECK_GE(record->cache_size, 0);
   statement.BindInt64(4, record->cache_size);
  DCHECK_GE(record->padding_size, 0);
  statement.BindInt64(5, record->padding_size);
 
   return statement.Run();
 }
