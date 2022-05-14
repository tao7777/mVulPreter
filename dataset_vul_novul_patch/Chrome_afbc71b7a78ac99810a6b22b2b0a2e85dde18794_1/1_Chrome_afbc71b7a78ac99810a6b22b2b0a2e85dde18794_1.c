OneClickSigninSyncStarter::OneClickSigninSyncStarter(
    Profile* profile,
    Browser* browser,
    const std::string& session_index,
    const std::string& email,
     const std::string& password,
     StartSyncMode start_mode,
     bool force_same_tab_navigation,
    ConfirmationRequired confirmation_required)
     : start_mode_(start_mode),
       force_same_tab_navigation_(force_same_tab_navigation),
       confirmation_required_(confirmation_required),
       weak_pointer_factory_(this) {
   DCHECK(profile);
   BrowserList::AddObserver(this);

  Initialize(profile, browser);

  SigninManager* manager = SigninManagerFactory::GetForProfile(profile_);
  SigninManager::OAuthTokenFetchedCallback callback;
  callback = base::Bind(&OneClickSigninSyncStarter::ConfirmSignin,
                        weak_pointer_factory_.GetWeakPtr());
  manager->StartSignInWithCredentials(session_index, email, password, callback);
}
