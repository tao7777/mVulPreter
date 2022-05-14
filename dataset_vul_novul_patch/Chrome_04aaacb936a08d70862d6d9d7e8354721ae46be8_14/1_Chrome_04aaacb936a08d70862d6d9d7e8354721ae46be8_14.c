 void AppCacheDatabase::ReadEntryRecord(
    const sql::Statement& statement, EntryRecord* record) {
  record->cache_id = statement.ColumnInt64(0);
  record->url = GURL(statement.ColumnString(1));
   record->flags = statement.ColumnInt(2);
   record->response_id = statement.ColumnInt64(3);
   record->response_size = statement.ColumnInt64(4);
 }
