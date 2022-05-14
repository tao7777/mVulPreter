 void ForeignSessionHelper::TriggerSessionSync(
     JNIEnv* env,
     const JavaParamRef<jobject>& obj) {
  browser_sync::ProfileSyncService* service =
      ProfileSyncServiceFactory::GetInstance()->GetForProfile(profile_);
   if (!service)
     return;
 
  const syncer::ModelTypeSet types(syncer::SESSIONS);
  service->TriggerRefresh(types);
 }
