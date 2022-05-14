bool AppCacheDatabase::InsertEntry(const EntryRecord* record) {
  if (!LazyOpen(kCreateIfNeeded))
     return false;
 
   static const char kSql[] =
      "INSERT INTO Entries (cache_id, url, flags, response_id, response_size)"
      "  VALUES(?, ?, ?, ?, ?)";
 
   sql::Statement statement(db_->GetCachedStatement(SQL_FROM_HERE, kSql));
   statement.BindInt64(0, record->cache_id);
   statement.BindString(1, record->url.spec());
   statement.BindInt(2, record->flags);
   statement.BindInt64(3, record->response_id);
   statement.BindInt64(4, record->response_size);
 
   return statement.Run();
 }
