void PeopleHandler::HandlePauseSync(const base::ListValue* args) {
  DCHECK(AccountConsistencyModeManager::IsDiceEnabledForProfile(profile_));
  SigninManager* signin_manager = SigninManagerFactory::GetForProfile(profile_);
   DCHECK(signin_manager->IsAuthenticated());
   ProfileOAuth2TokenServiceFactory::GetForProfile(profile_)->UpdateCredentials(
       signin_manager->GetAuthenticatedAccountId(),
      OAuth2TokenServiceDelegate::kInvalidRefreshToken,
      signin_metrics::SourceForRefreshTokenOperation::kSettings_PauseSync);
 }
