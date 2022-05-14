void InlineSigninHelper::OnClientOAuthSuccessAndBrowserOpened(
    const ClientOAuthResult& result,
    Profile* profile,
    Profile::CreateStatus status) {
  if (is_force_sign_in_with_usermanager_)
    UnlockProfileAndHideLoginUI(profile_->GetPath(), handler_.get());
  Browser* browser = NULL;
  if (handler_) {
    browser = handler_->GetDesktopBrowser();
  }

  AboutSigninInternals* about_signin_internals =
      AboutSigninInternalsFactory::GetForProfile(profile_);
  about_signin_internals->OnRefreshTokenReceived("Successful");

  std::string account_id =
      AccountTrackerServiceFactory::GetForProfile(profile_)
          ->SeedAccountInfo(gaia_id_, email_);

  signin_metrics::AccessPoint access_point =
      signin::GetAccessPointForPromoURL(current_url_);
  signin_metrics::Reason reason =
      signin::GetSigninReasonForPromoURL(current_url_);

  SigninManager* signin_manager = SigninManagerFactory::GetForProfile(profile_);
  std::string primary_email =
      signin_manager->GetAuthenticatedAccountInfo().email;
  if (gaia::AreEmailsSame(email_, primary_email) &&
      (reason == signin_metrics::Reason::REASON_REAUTHENTICATION ||
       reason == signin_metrics::Reason::REASON_UNLOCK) &&
      !password_.empty() && profiles::IsLockAvailable(profile_)) {
    LocalAuth::SetLocalAuthCredentials(profile_, password_);
  }

#if defined(SYNC_PASSWORD_REUSE_DETECTION_ENABLED)
  if (!password_.empty()) {
    scoped_refptr<password_manager::PasswordStore> password_store =
        PasswordStoreFactory::GetForProfile(profile_,
                                            ServiceAccessType::EXPLICIT_ACCESS);
    if (password_store && !primary_email.empty()) {
      password_store->SaveGaiaPasswordHash(
          primary_email, base::UTF8ToUTF16(password_),
          password_manager::metrics_util::SyncPasswordHashChange::
              SAVED_ON_CHROME_SIGNIN);
    }
  }
#endif

   if (reason == signin_metrics::Reason::REASON_REAUTHENTICATION ||
       reason == signin_metrics::Reason::REASON_UNLOCK ||
       reason == signin_metrics::Reason::REASON_ADD_SECONDARY_ACCOUNT) {
    ProfileOAuth2TokenServiceFactory::GetForProfile(profile_)
        ->UpdateCredentials(account_id, result.refresh_token,
                            signin_metrics::SourceForRefreshTokenOperation::
                                kInlineLoginHandler_Signin);
 
     if (signin::IsAutoCloseEnabledInURL(current_url_)) {
      bool show_account_management = ShouldShowAccountManagement(
          current_url_,
          AccountConsistencyModeManager::IsMirrorEnabledForProfile(profile_));
      base::ThreadTaskRunnerHandle::Get()->PostTask(
          FROM_HERE, base::BindOnce(&InlineLoginHandlerImpl::CloseTab, handler_,
                                    show_account_management));
    }

    if (reason == signin_metrics::Reason::REASON_REAUTHENTICATION ||
        reason == signin_metrics::Reason::REASON_UNLOCK) {
      signin_manager->MergeSigninCredentialIntoCookieJar();
    }
    LogSigninReason(reason);
  } else {
    browser_sync::ProfileSyncService* sync_service =
        ProfileSyncServiceFactory::GetForProfile(profile_);
    SigninErrorController* error_controller =
        SigninErrorControllerFactory::GetForProfile(profile_);

    OneClickSigninSyncStarter::StartSyncMode start_mode =
        OneClickSigninSyncStarter::CONFIRM_SYNC_SETTINGS_FIRST;
    if (access_point == signin_metrics::AccessPoint::ACCESS_POINT_SETTINGS ||
        choose_what_to_sync_) {
      bool show_settings_without_configure =
          error_controller->HasError() && sync_service &&
          sync_service->IsFirstSetupComplete();
      if (!show_settings_without_configure)
        start_mode = OneClickSigninSyncStarter::CONFIGURE_SYNC_FIRST;
    }

    OneClickSigninSyncStarter::ConfirmationRequired confirmation_required =
        confirm_untrusted_signin_ ?
            OneClickSigninSyncStarter::CONFIRM_UNTRUSTED_SIGNIN :
            OneClickSigninSyncStarter::CONFIRM_AFTER_SIGNIN;

    bool start_signin = !HandleCrossAccountError(
        result.refresh_token, confirmation_required, start_mode);
    if (start_signin) {
      CreateSyncStarter(browser, current_url_, result.refresh_token,
                        OneClickSigninSyncStarter::CURRENT_PROFILE, start_mode,
                        confirmation_required);
      base::ThreadTaskRunnerHandle::Get()->DeleteSoon(FROM_HERE, this);
    }
  }
}
