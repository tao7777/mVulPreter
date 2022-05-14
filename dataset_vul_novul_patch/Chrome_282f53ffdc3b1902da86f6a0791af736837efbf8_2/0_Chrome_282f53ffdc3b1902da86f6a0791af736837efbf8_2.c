 void SupervisedUserService::InitSync(const std::string& refresh_token) {
   ProfileOAuth2TokenService* token_service =
       ProfileOAuth2TokenServiceFactory::GetForProfile(profile_);
  token_service->UpdateCredentials(
      supervised_users::kSupervisedUserPseudoEmail, refresh_token,
      signin_metrics::SourceForRefreshTokenOperation::kSupervisedUser_InitSync);
 }
