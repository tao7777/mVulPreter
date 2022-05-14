void DatabaseMessageFilter::OnHandleSqliteError(
    const string16& origin_identifier,
     const string16& database_name,
     int error) {
   DCHECK(BrowserThread::CurrentlyOn(BrowserThread::FILE));
  if (!DatabaseUtil::IsValidOriginIdentifier(origin_identifier)) {
    RecordAction(UserMetricsAction("BadMessageTerminate_DBMF"));
    BadMessageReceived();
    return;
  }

   db_tracker_->HandleSqliteError(origin_identifier, database_name, error);
 }
