 void ProfileChooserView::SignOutAllWebAccounts() {
   Hide();
   ProfileOAuth2TokenServiceFactory::GetForProfile(browser_->profile())
      ->RevokeAllCredentials(signin_metrics::SourceForRefreshTokenOperation::
                                 kUserMenu_SignOutAllAccounts);
 }
