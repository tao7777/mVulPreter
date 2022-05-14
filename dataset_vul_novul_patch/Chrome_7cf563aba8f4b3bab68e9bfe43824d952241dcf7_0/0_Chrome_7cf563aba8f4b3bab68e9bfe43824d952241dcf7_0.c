 void GaiaOAuthClient::Core::FetchUserInfoAndInvokeCallback() {
  request_.reset(net::URLFetcher::Create(
      GURL(provider_info_.user_info_url), net::URLFetcher::GET, this));
   request_->SetRequestContext(request_context_getter_);
  request_->AddExtraRequestHeader("Authorization: Bearer " + access_token_);
  url_fetcher_type_ = URL_FETCHER_GET_USER_INFO;
  request_->Start();
 }
