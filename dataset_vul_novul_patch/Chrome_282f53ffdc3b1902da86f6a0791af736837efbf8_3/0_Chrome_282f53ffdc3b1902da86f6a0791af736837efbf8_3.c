void ProfileChooserView::RemoveAccount() {
  DCHECK(!account_id_to_remove_.empty());
   ProfileOAuth2TokenService* oauth2_token_service =
       ProfileOAuth2TokenServiceFactory::GetForProfile(browser_->profile());
   if (oauth2_token_service) {
    oauth2_token_service->RevokeCredentials(
        account_id_to_remove_, signin_metrics::SourceForRefreshTokenOperation::
                                   kUserMenu_RemoveAccount);
     PostActionPerformed(ProfileMetrics::PROFILE_DESKTOP_MENU_REMOVE_ACCT);
   }
   account_id_to_remove_.clear();

  ShowViewFromMode(profiles::BUBBLE_VIEW_MODE_ACCOUNT_MANAGEMENT);
}
