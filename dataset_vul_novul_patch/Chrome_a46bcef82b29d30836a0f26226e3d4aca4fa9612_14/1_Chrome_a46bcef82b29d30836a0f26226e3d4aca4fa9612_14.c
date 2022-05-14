void ChromotingHost::Start() {
  if (!context_->network_message_loop()->BelongsToCurrentThread()) {
    context_->network_message_loop()->PostTask(
        FROM_HERE, base::Bind(&ChromotingHost::Start, this));
    return;
  }

  LOG(INFO) << "Starting host";
  DCHECK(!signal_strategy_.get());
   DCHECK(access_verifier_.get());
 
  {
    base::AutoLock auto_lock(lock_);
    if (state_ != kInitial)
      return;
    state_ = kStarted;
  }
 
   std::string xmpp_login;
  std::string xmpp_auth_token;
  std::string xmpp_auth_service;
  if (!config_->GetString(kXmppLoginConfigPath, &xmpp_login) ||
      !config_->GetString(kXmppAuthTokenConfigPath, &xmpp_auth_token) ||
      !config_->GetString(kXmppAuthServiceConfigPath, &xmpp_auth_service)) {
    LOG(ERROR) << "XMPP credentials are not defined in the config.";
    return;
  }

  signal_strategy_.reset(
      new XmppSignalStrategy(context_->jingle_thread(), xmpp_login,
                             xmpp_auth_token,
                             xmpp_auth_service));
  signal_strategy_->Init(this);
}
