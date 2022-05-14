void OneClickSigninHelper::ShowInfoBarIfPossible(net::URLRequest* request,
                                                 ProfileIOData* io_data,
                                                 int child_id,
                                                 int route_id) {
  std::string google_chrome_signin_value;
  std::string google_accounts_signin_value;
  request->GetResponseHeaderByName("Google-Chrome-SignIn",
                                   &google_chrome_signin_value);
  request->GetResponseHeaderByName("Google-Accounts-SignIn",
                                   &google_accounts_signin_value);

  if (!google_accounts_signin_value.empty() ||
      !google_chrome_signin_value.empty()) {
    VLOG(1) << "OneClickSigninHelper::ShowInfoBarIfPossible:"
            << " g-a-s='" << google_accounts_signin_value << "'"
             << " g-c-s='" << google_chrome_signin_value << "'";
   }
 
  if (!gaia::IsGaiaSignonRealm(request->url().GetOrigin()))
     return;
 
  std::vector<std::pair<std::string, std::string> > pairs;
  base::SplitStringIntoKeyValuePairs(google_accounts_signin_value, '=', ',',
                                     &pairs);
  std::string session_index;
  std::string email;
  for (size_t i = 0; i < pairs.size(); ++i) {
    const std::pair<std::string, std::string>& pair = pairs[i];
    const std::string& key = pair.first;
    const std::string& value = pair.second;
    if (key == "email") {
      TrimString(value, "\"", &email);
    } else if (key == "sessionindex") {
      session_index = value;
    }
  }

  if (!email.empty())
    io_data->set_reverse_autologin_pending_email(email);

  if (!email.empty() || !session_index.empty()) {
    VLOG(1) << "OneClickSigninHelper::ShowInfoBarIfPossible:"
            << " email=" << email
            << " sessionindex=" << session_index;
  }

  AutoAccept auto_accept = AUTO_ACCEPT_NONE;
  signin::Source source = signin::SOURCE_UNKNOWN;
  GURL continue_url;
  std::vector<std::string> tokens;
  base::SplitString(google_chrome_signin_value, ',', &tokens);
  for (size_t i = 0; i < tokens.size(); ++i) {
    const std::string& token = tokens[i];
    if (token == "accepted") {
      auto_accept = AUTO_ACCEPT_ACCEPTED;
    } else if (token == "configure") {
      auto_accept = AUTO_ACCEPT_CONFIGURE;
    } else if (token == "rejected-for-profile") {
      auto_accept = AUTO_ACCEPT_REJECTED_FOR_PROFILE;
    }
  }

  source = GetSigninSource(request->url(), &continue_url);
  if (source != signin::SOURCE_UNKNOWN)
    auto_accept = AUTO_ACCEPT_EXPLICIT;

  if (auto_accept != AUTO_ACCEPT_NONE) {
    VLOG(1) << "OneClickSigninHelper::ShowInfoBarIfPossible:"
            << " auto_accept=" << auto_accept;
  }

  if (session_index.empty() && email.empty() &&
      auto_accept == AUTO_ACCEPT_NONE && !continue_url.is_valid()) {
    return;
  }

  content::BrowserThread::PostTask(
      content::BrowserThread::UI, FROM_HERE,
      base::Bind(&OneClickSigninHelper::ShowInfoBarUIThread, session_index,
                 email, auto_accept, source, continue_url, child_id, route_id));
}
