void ForeignSessionHelper::SetInvalidationsForSessionsEnabled(
     JNIEnv* env,
     const JavaParamRef<jobject>& obj,
     jboolean enabled) {
  syncer::SyncService* service =
      ProfileSyncServiceFactory::GetSyncServiceForProfile(profile_);
   if (!service)
     return;
 
  service->SetInvalidationsForSessionsEnabled(enabled);
}
