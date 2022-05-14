DirOpenResult DirectoryBackingStore::InitializeTables() {
  sqlite_utils::SQLTransaction transaction(load_dbhandle_);
  if (SQLITE_OK != transaction.BeginExclusive()) {
    return FAILED_DISK_FULL;
  }
  int version_on_disk = GetVersion();
  int last_result = SQLITE_DONE;

  if (version_on_disk == 67) {
    if (MigrateVersion67To68())
      version_on_disk = 68;
  }
  if (version_on_disk == 68) {
    if (MigrateVersion68To69())
      version_on_disk = 69;
  }

  if (version_on_disk == 69) {
    if (MigrateVersion69To70())
      version_on_disk = 70;
  }

  if (version_on_disk == 70) {
    if (MigrateVersion70To71())
      version_on_disk = 71;
  }

  if (version_on_disk == 71) {
    if (MigrateVersion71To72())
      version_on_disk = 72;
  }

  if (version_on_disk == 72) {
    if (MigrateVersion72To73())
      version_on_disk = 73;
  }

  if (version_on_disk == 73) {
    if (MigrateVersion73To74())
      version_on_disk = 74;
  }

  if (version_on_disk == 74) {
    if (MigrateVersion74To75())
      version_on_disk = 75;
  }

  if (version_on_disk == 75) {
    if (MigrateVersion75To76())
       version_on_disk = 76;
   }
 
  if (version_on_disk == 76) {
    if (MigrateVersion76To77())
      version_on_disk = 77;
  }
  if (version_on_disk == kCurrentDBVersion && needs_column_refresh_) {
    if (!RefreshColumns())
      version_on_disk = 0;
  }

  if (version_on_disk != kCurrentDBVersion) {
    if (version_on_disk > kCurrentDBVersion) {
      transaction.Rollback();
      return FAILED_NEWER_VERSION;
    }
    VLOG(1) << "Old/null sync database, version " << version_on_disk;
    DropAllTables();
    last_result = CreateTables();
  }
  if (SQLITE_DONE == last_result) {
    {
      sqlite_utils::SQLStatement statement;
      statement.prepare(load_dbhandle_,
          "SELECT db_create_version, db_create_time FROM share_info");
      if (SQLITE_ROW != statement.step()) {
        transaction.Rollback();
        return FAILED_DISK_FULL;
      }
      string db_create_version = statement.column_text(0);
      int db_create_time = statement.column_int(1);
      statement.reset();
      VLOG(1) << "DB created at " << db_create_time << " by version " <<
          db_create_version;
    }
    if (SQLITE_OK == transaction.Commit())
      return OPENED;
  } else {
    transaction.Rollback();
  }
  return FAILED_DISK_FULL;
}
