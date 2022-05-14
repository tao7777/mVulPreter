 void GaiaOAuthClient::Core::OnAuthTokenFetchComplete(
    const net::URLRequestStatus& status,
    int response_code,
    const std::string& response) {
  request_.reset();

  if (!status.is_success()) {
    delegate_->OnNetworkError(response_code);
    return;
  }

  if (response_code == net::HTTP_BAD_REQUEST) {
    LOG(ERROR) << "Gaia response: response code=net::HTTP_BAD_REQUEST.";
    delegate_->OnOAuthError();
    return;
  }

  if (response_code == net::HTTP_OK) {
    scoped_ptr<Value> message_value(base::JSONReader::Read(response));
    if (message_value.get() &&
         message_value->IsType(Value::TYPE_DICTIONARY)) {
       scoped_ptr<DictionaryValue> response_dict(
           static_cast<DictionaryValue*>(message_value.release()));
      std::string access_token;
      response_dict->GetString(kAccessTokenValue, &access_token);
      if (access_token.find("\r\n") != std::string::npos) {
        LOG(ERROR) << "Gaia response: access token include CRLF";
        delegate_->OnOAuthError();
        return;
      }
      access_token_ = access_token;
       response_dict->GetInteger(kExpiresInValue, &expires_in_seconds_);
     }
     VLOG(1) << "Gaia response: acess_token='" << access_token_
            << "', expires in " << expires_in_seconds_ << " second(s)";
  } else {
    LOG(ERROR) << "Gaia response: response code=" << response_code;
  }

  if (access_token_.empty()) {
    delegate_->OnNetworkError(response_code);
  } else {
    FetchUserInfoAndInvokeCallback();
  }
 }
