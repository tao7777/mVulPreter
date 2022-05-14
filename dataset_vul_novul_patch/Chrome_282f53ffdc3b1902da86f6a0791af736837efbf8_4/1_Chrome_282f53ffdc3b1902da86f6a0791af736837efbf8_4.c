 void ProfileChooserView::SignOutAllWebAccounts() {
   Hide();
   ProfileOAuth2TokenServiceFactory::GetForProfile(browser_->profile())
      ->RevokeAllCredentials();
 }
