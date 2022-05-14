 void GaiaOAuthClient::Core::FetchUserInfoAndInvokeCallback() {
  request_.reset(new UrlFetcher(
      GURL(provider_info_.user_info_url), UrlFetcher::GET));
   request_->SetRequestContext(request_context_getter_);
  request_->SetHeader("Authorization", "Bearer " + access_token_);
  request_->Start(
      base::Bind(&GaiaOAuthClient::Core::OnUserInfoFetchComplete, this));
 }
