bool AppCacheDatabase::FindEntry(int64_t cache_id,
                                 const GURL& url,
                                 EntryRecord* record) {
  DCHECK(record);
  if (!LazyOpen(kDontCreate))
     return false;
 
   static const char kSql[] =
      "SELECT cache_id, url, flags, response_id, response_size, padding_size "
      "FROM Entries"
       "  WHERE cache_id = ? AND url = ?";
 
   sql::Statement statement(db_->GetCachedStatement(SQL_FROM_HERE, kSql));
  statement.BindInt64(0, cache_id);
  statement.BindString(1, url.spec());

  if (!statement.Step())
    return false;

  ReadEntryRecord(statement, record);
  DCHECK(record->cache_id == cache_id);
  DCHECK(record->url == url);
  return true;
}
