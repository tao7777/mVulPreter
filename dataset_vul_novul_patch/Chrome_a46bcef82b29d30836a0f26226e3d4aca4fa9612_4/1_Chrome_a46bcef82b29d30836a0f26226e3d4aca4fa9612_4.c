void ChromotingHost::OnIncomingSession(
      protocol::Session* session,
       protocol::SessionManager::IncomingSessionResponse* response) {
   DCHECK(context_->network_message_loop()->BelongsToCurrentThread());
 
  base::AutoLock auto_lock(lock_);
   if (state_ != kStarted) {
     *response = protocol::SessionManager::DECLINE;
     return;
  }

  if (!access_verifier_->VerifyPermissions(session->jid(),
                                           session->initiator_token())) {
    *response = protocol::SessionManager::DECLINE;

    for (StatusObserverList::iterator it = status_observers_.begin();
         it != status_observers_.end(); ++it) {
      (*it)->OnAccessDenied();
    }
    return;
  }

  if (is_it2me_ && AuthenticatedClientsCount() > 0) {
    *response = protocol::SessionManager::DECLINE;

    Shutdown(base::Closure());
    return;
  }

  *protocol_config_->mutable_initial_resolution() =
      protocol::ScreenResolution(2048, 2048);
  protocol::SessionConfig config;
  if (!protocol_config_->Select(session->candidate_config(),
                                true /* force_host_resolution */, &config)) {
    LOG(WARNING) << "Rejecting connection from " << session->jid()
                 << " because no compatible configuration has been found.";
    *response = protocol::SessionManager::INCOMPATIBLE;
    return;
  }

  session->set_config(config);
  session->set_receiver_token(
      GenerateHostAuthToken(session->initiator_token()));

  session->set_shared_secret(access_code_);

  *response = protocol::SessionManager::ACCEPT;

  LOG(INFO) << "Client connected: " << session->jid();

  scoped_refptr<protocol::ConnectionToClient> connection =
       new protocol::ConnectionToClient(context_->network_message_loop(),
                                        session);
   ClientSession* client = new ClientSession(
      this, connection,
      desktop_environment_->event_executor(),
       desktop_environment_->capturer());
   clients_.push_back(client);
 }
