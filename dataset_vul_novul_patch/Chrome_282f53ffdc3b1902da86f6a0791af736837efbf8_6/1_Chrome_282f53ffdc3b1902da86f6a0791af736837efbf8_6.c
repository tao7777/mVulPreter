void PeopleHandler::HandleSignout(const base::ListValue* args) {
  bool delete_profile = false;
  args->GetBoolean(0, &delete_profile);

  if (!signin_util::IsUserSignoutAllowedForProfile(profile_)) {
    DCHECK(delete_profile);
  } else {
    SigninManager* signin_manager =
        SigninManagerFactory::GetForProfile(profile_);
    if (signin_manager->IsAuthenticated()) {
      if (GetSyncService())
        ProfileSyncService::SyncEvent(ProfileSyncService::STOP_FROM_OPTIONS);

      signin_metrics::SignoutDelete delete_metric =
          delete_profile ? signin_metrics::SignoutDelete::DELETED
                         : signin_metrics::SignoutDelete::KEEPING;
      signin_manager->SignOutAndRemoveAllAccounts(
          signin_metrics::USER_CLICKED_SIGNOUT_SETTINGS, delete_metric);
    } else {
       DCHECK(!delete_profile)
           << "Deleting the profile should only be offered the user is syncing.";
       ProfileOAuth2TokenServiceFactory::GetForProfile(profile_)
          ->RevokeAllCredentials();
     }
   }
 
  if (delete_profile) {
    webui::DeleteProfileAtPath(profile_->GetPath(),
                               ProfileMetrics::DELETE_PROFILE_SETTINGS);
  }
}
