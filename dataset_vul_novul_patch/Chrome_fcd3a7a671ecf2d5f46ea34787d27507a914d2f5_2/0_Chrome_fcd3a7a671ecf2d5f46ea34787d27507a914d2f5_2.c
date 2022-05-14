 void ProfileSyncService::RegisterNewDataType(syncable::ModelType data_type) {
   if (data_type_controllers_.count(data_type) > 0)
     return;
   NOTREACHED();
 }
