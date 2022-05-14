void BrowserPolicyConnector::SetDeviceCredentials(
     const std::string& owner_email,
     const std::string& token,
     TokenType token_type) {
#if defined(OS_CHROMEOS)
  if (device_data_store_.get()) {
    device_data_store_->set_user_name(owner_email);
    switch (token_type) {
      case TOKEN_TYPE_OAUTH:
        device_data_store_->SetOAuthToken(token);
        break;
      case TOKEN_TYPE_GAIA:
        device_data_store_->SetGaiaToken(token);
        break;
      default:
        NOTREACHED() << "Invalid token type " << token_type;
    }
  }
#endif
}
