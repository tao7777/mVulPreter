  void HandleCompleteLogin(const base::ListValue* args) {
#if defined(OS_CHROMEOS)
    oauth2_delegate_.reset(new InlineLoginUIOAuth2Delegate(web_ui()));
    oauth2_token_fetcher_.reset(new chromeos::OAuth2TokenFetcher(
        oauth2_delegate_.get(), profile_->GetRequestContext()));
    oauth2_token_fetcher_->StartExchangeFromCookies();
#elif !defined(OS_ANDROID)
    const base::DictionaryValue* dict = NULL;
    string16 email;
    string16 password;
    if (!args->GetDictionary(0, &dict) || !dict ||
        !dict->GetString("email", &email) ||
        !dict->GetString("password", &password)) {
      NOTREACHED();
      return;
    }

    new OneClickSigninSyncStarter(
        profile_, NULL, "0" /* session_index 0 for the default user */,
         UTF16ToASCII(email), UTF16ToASCII(password),
         OneClickSigninSyncStarter::SYNC_WITH_DEFAULT_SETTINGS,
         true /* force_same_tab_navigation */,
        OneClickSigninSyncStarter::NO_CONFIRMATION);
     web_ui()->CallJavascriptFunction("inline.login.closeDialog");
 #endif
   }
