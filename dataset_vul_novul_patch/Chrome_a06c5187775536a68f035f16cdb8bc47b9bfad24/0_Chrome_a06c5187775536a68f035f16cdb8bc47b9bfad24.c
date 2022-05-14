bool SQLiteDatabase::Open(const String& filename) {
  Close();

  open_error_ = SQLiteFileSystem::OpenDatabase(filename, &db_);
  if (open_error_ != SQLITE_OK) {
    open_error_message_ =
        db_ ? sqlite3_errmsg(db_) : "sqlite_open returned null";
    DLOG(ERROR) << "SQLite database failed to load from " << filename
                << "\nCause - " << open_error_message_.data();
    sqlite3_close(db_);
    db_ = nullptr;
    return false;
  }

  open_error_ = sqlite3_extended_result_codes(db_, 1);
  if (open_error_ != SQLITE_OK) {
    open_error_message_ = sqlite3_errmsg(db_);
    DLOG(ERROR) << "SQLite database error when enabling extended errors - "
                << open_error_message_.data();
    sqlite3_close(db_);
    db_ = nullptr;
     return false;
   }
 
  // Defensive mode is a layer of defense in depth for applications that must
  // run SQL queries from an untrusted source, such as WebDatabase. Refuse to
  // proceed if this layer cannot be enabled.
  open_error_ = sqlite3_db_config(db_, SQLITE_DBCONFIG_DEFENSIVE, 1, nullptr);
  if (open_error_ != SQLITE_OK) {
    open_error_message_ = sqlite3_errmsg(db_);
    DLOG(ERROR) << "SQLite database error when enabling defensive mode - "
                << open_error_message_.data();
    sqlite3_close(db_);
    db_ = nullptr;
    return false;
  }

   if (IsOpen())
     opening_thread_ = CurrentThread();
   else
    open_error_message_ = "sqlite_open returned null";

  if (!SQLiteStatement(*this, "PRAGMA temp_store = MEMORY;").ExecuteCommand())
    DLOG(ERROR) << "SQLite database could not set temp_store to memory";

  if (!SQLiteStatement(*this, "PRAGMA foreign_keys = OFF;").ExecuteCommand())
    DLOG(ERROR) << "SQLite database could not turn off foreign_keys";

  return IsOpen();
}
