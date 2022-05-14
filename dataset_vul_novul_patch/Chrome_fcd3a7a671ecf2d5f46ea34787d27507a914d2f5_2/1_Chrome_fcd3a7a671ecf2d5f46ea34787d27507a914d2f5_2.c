 void ProfileSyncService::RegisterNewDataType(syncable::ModelType data_type) {
   if (data_type_controllers_.count(data_type) > 0)
     return;
  switch (data_type) {
    case syncable::SESSIONS:
      if (CommandLine::ForCurrentProcess()->HasSwitch(
              switches::kDisableSyncTabs)) {
        return;
      }
      RegisterDataTypeController(
          new browser_sync::SessionDataTypeController(factory_.get(),
                                                      profile_,
                                                      this));
      return;
    default:
      break;
  }
   NOTREACHED();
 }
