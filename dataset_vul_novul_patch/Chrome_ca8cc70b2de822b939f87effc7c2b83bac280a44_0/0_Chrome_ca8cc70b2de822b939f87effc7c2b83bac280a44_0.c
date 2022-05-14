 void SocketStream::CheckPrivacyMode() {
  if (context_ && context_->network_delegate()) {
     bool enable = context_->network_delegate()->CanEnablePrivacyMode(url_,
                                                                      url_);
     privacy_mode_ = enable ? kPrivacyModeEnabled : kPrivacyModeDisabled;
    if (enable)
      server_ssl_config_.channel_id_enabled = false;
  }
}
