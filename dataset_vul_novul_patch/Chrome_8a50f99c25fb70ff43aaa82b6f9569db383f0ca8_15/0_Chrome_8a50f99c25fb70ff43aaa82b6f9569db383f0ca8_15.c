   virtual void SetUp() {
    StartClient();

    registered_ids_.insert(kBookmarksId_);
    registered_ids_.insert(kPreferencesId_);
    client_.UpdateRegisteredIds(registered_ids_);
   }
