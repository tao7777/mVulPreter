bool AppCacheDatabase::FindEntriesForUrl(
    const GURL& url, std::vector<EntryRecord>* records) {
  DCHECK(records && records->empty());
  if (!LazyOpen(kDontCreate))
     return false;
 
   static const char kSql[] =
      "SELECT cache_id, url, flags, response_id, response_size FROM Entries"
       "  WHERE url = ?";
 
   sql::Statement statement(db_->GetCachedStatement(SQL_FROM_HERE, kSql));
  statement.BindString(0, url.spec());

  while (statement.Step()) {
    records->push_back(EntryRecord());
    ReadEntryRecord(statement, &records->back());
    DCHECK(records->back().url == url);
  }

  return statement.Succeeded();
}
