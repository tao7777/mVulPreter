 bool AppCacheDatabase::UpgradeSchema() {
  // Start from scratch for versions that would require unsupported migrations.
  if (meta_table_->GetVersionNumber() < 7)
    return DeleteExistingAndCreateNewDatabase();

  sql::Transaction transaction(db_.get());
  if (!transaction.Begin())
    return false;
  if (!db_->Execute("ALTER TABLE Caches ADD COLUMN padding_size INTEGER"))
    return false;
  if (!db_->Execute("ALTER TABLE Entries ADD COLUMN padding_size INTEGER"))
    return false;
  meta_table_->SetVersionNumber(8);
  meta_table_->SetCompatibleVersionNumber(8);
  if (!AppCacheBackfillerVersion8(db_.get()).BackfillPaddingSizes())
    return false;
  return transaction.Commit();
 }
