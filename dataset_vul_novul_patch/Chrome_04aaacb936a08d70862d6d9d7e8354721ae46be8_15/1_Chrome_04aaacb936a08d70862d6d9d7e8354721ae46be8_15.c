 bool AppCacheDatabase::UpgradeSchema() {
  return DeleteExistingAndCreateNewDatabase();
 }
