void GaiaOAuthClient::Core::RefreshToken(
    const OAuthClientInfo& oauth_client_info,
    const std::string& refresh_token,
    GaiaOAuthClient::Delegate* delegate) {
  DCHECK(!request_.get()) << "Tried to fetch two things at once!";

  delegate_ = delegate;

  access_token_.clear();
  expires_in_seconds_ = 0;

  std::string post_body =
      "refresh_token=" + net::EscapeUrlEncodedData(refresh_token, true) +
      "&client_id=" + net::EscapeUrlEncodedData(oauth_client_info.client_id,
                                                true) +
       "&client_secret=" +
       net::EscapeUrlEncodedData(oauth_client_info.client_secret, true) +
       "&grant_type=refresh_token";
  request_.reset(net::URLFetcher::Create(
      GURL(provider_info_.access_token_url), net::URLFetcher::POST, this));
   request_->SetRequestContext(request_context_getter_);
   request_->SetUploadData("application/x-www-form-urlencoded", post_body);
  url_fetcher_type_ = URL_FETCHER_REFRESH_TOKEN;
  request_->Start();
}

void GaiaOAuthClient::Core::OnURLFetchComplete(
    const net::URLFetcher* source) {
  std::string response_string;
  source->GetResponseAsString(&response_string);
  switch (url_fetcher_type_) {
    case URL_FETCHER_REFRESH_TOKEN:
      OnAuthTokenFetchComplete(source->GetStatus(),
                              source->GetResponseCode(),
                              response_string);
      break;
    case URL_FETCHER_GET_USER_INFO:
      OnUserInfoFetchComplete(source->GetStatus(),
                              source->GetResponseCode(),
                              response_string);
      break;
    default:
      LOG(ERROR) << "Unrecognised URLFetcher type: " << url_fetcher_type_;
  }
 }
