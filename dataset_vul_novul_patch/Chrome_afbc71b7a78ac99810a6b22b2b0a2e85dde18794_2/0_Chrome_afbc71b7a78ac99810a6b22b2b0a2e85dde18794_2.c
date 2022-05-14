void OneClickSigninSyncStarter::UntrustedSigninConfirmed(
     StartSyncMode response) {
   if (response == UNDO_SYNC) {
     CancelSigninAndDelete();
    // If this was not an interstitial signin, (i.e. it was a SAML signin)
    // then the browser page is now blank and should redirect to the NTP.
    if (source_ != SyncPromoUI::SOURCE_UNKNOWN) {
      EnsureBrowser();
      chrome::NavigateParams params(browser_, GURL(chrome::kChromeUINewTabURL),
                                    content::PAGE_TRANSITION_AUTO_TOPLEVEL);
      params.disposition = CURRENT_TAB;
      params.window_action = chrome::NavigateParams::SHOW_WINDOW;
      chrome::Navigate(&params);
    }
   } else {
    if (response == CONFIGURE_SYNC_FIRST)
      start_mode_ = response;
    SigninManager* signin = SigninManagerFactory::GetForProfile(profile_);
    signin->CompletePendingSignin();
  }
}
