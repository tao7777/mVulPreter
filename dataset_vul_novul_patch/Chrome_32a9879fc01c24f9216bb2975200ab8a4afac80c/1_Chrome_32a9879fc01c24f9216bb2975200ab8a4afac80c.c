void ForeignSessionHelper::SetInvalidationsForSessionsEnabled(
     JNIEnv* env,
     const JavaParamRef<jobject>& obj,
     jboolean enabled) {
  browser_sync::ProfileSyncService* service =
      ProfileSyncServiceFactory::GetInstance()->GetForProfile(profile_);
   if (!service)
     return;
 
  service->SetInvalidationsForSessionsEnabled(enabled);
}
