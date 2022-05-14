void AppCacheDatabase::ReadCacheRecord(
    const sql::Statement& statement, CacheRecord* record) {
  record->cache_id = statement.ColumnInt64(0);
  record->group_id = statement.ColumnInt64(1);
  record->online_wildcard = statement.ColumnBool(2);
   record->update_time =
       base::Time::FromInternalValue(statement.ColumnInt64(3));
   record->cache_size = statement.ColumnInt64(4);
  record->padding_size = statement.ColumnInt64(5);
 }
