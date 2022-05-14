void ChromotingHost::AddAuthenticatedClient(
    ClientSession* client,
    const protocol::SessionConfig& config,
    const std::string& jid) {
  DCHECK_EQ(context_->main_message_loop(), MessageLoop::current());
  ClientList clients_copy(clients_);
  for (ClientList::const_iterator other_client = clients_copy.begin();
       other_client != clients_copy.end(); ++other_client) {
    if ((*other_client) != client) {
      (*other_client)->Disconnect();
      OnClientDisconnected(*other_client);
    }
  }
  CHECK(recorder_.get() == NULL);
  if (!recorder_.get()) {
    Encoder* encoder = CreateEncoder(config);
    recorder_ = new ScreenRecorder(context_->main_message_loop(),
                                   context_->encode_message_loop(),
                                   context_->network_message_loop(),
                                   desktop_environment_->capturer(),
                                   encoder);
  }
  recorder_->AddConnection(client->connection());
  recorder_->Start();
  for (StatusObserverList::iterator it = status_observers_.begin();
       it != status_observers_.end(); ++it) {
    (*it)->OnClientAuthenticated(jid);
  }
  EnableCurtainMode(true);
  if (is_it2me_) {
    std::string username = jid;
    size_t pos = username.find('/');
    if (pos != std::string::npos)
      username.replace(pos, std::string::npos, "");
    desktop_environment_->OnConnect(username);
  }
}
