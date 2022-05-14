void DatabaseMessageFilter::OnHandleSqliteError(
    const string16& origin_identifier,
     const string16& database_name,
     int error) {
   DCHECK(BrowserThread::CurrentlyOn(BrowserThread::FILE));
   db_tracker_->HandleSqliteError(origin_identifier, database_name, error);
 }
