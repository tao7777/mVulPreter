 void ForeignSessionHelper::TriggerSessionSync(
     JNIEnv* env,
     const JavaParamRef<jobject>& obj) {
  syncer::SyncService* service =
      ProfileSyncServiceFactory::GetSyncServiceForProfile(profile_);
   if (!service)
     return;
 
  service->TriggerRefresh({syncer::SESSIONS});
 }
