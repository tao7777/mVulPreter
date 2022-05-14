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
  request_.reset(new UrlFetcher(GURL(provider_info_.access_token_url),
                                UrlFetcher::POST));
   request_->SetRequestContext(request_context_getter_);
   request_->SetUploadData("application/x-www-form-urlencoded", post_body);
  request_->Start(
      base::Bind(&GaiaOAuthClient::Core::OnAuthTokenFetchComplete, this));
 }
