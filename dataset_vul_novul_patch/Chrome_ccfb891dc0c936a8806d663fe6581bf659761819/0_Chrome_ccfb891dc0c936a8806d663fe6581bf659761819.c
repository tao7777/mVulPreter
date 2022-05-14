void DatabaseMessageFilter::OnDatabaseOpened(const string16& origin_identifier,
                                             const string16& database_name,
                                              const string16& description,
                                              int64 estimated_size) {
   DCHECK(BrowserThread::CurrentlyOn(BrowserThread::FILE));

  if (!DatabaseUtil::IsValidOriginIdentifier(origin_identifier)) {
    RecordAction(UserMetricsAction("BadMessageTerminate_DBMF"));
    BadMessageReceived();
    return;
  }

   int64 database_size = 0;
   db_tracker_->DatabaseOpened(origin_identifier, database_name, description,
                               estimated_size, &database_size);
  database_connections_.AddConnection(origin_identifier, database_name);
  Send(new DatabaseMsg_UpdateSize(origin_identifier, database_name,
                                  database_size));
}
