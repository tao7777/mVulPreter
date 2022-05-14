void OneClickSigninSyncStarter::UntrustedSigninConfirmed(
     StartSyncMode response) {
   if (response == UNDO_SYNC) {
     CancelSigninAndDelete();
   } else {
    if (response == CONFIGURE_SYNC_FIRST)
      start_mode_ = response;
    SigninManager* signin = SigninManagerFactory::GetForProfile(profile_);
    signin->CompletePendingSignin();
  }
}
