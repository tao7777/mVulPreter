 void ChromotingHost::OnSessionAuthenticated(ClientSession* client) {
   DCHECK(context_->network_message_loop()->BelongsToCurrentThread());

  // Disconnect all other clients.
  // Iterate over a copy of the list of clients, to avoid mutating the list
  // while iterating over it.
  ClientList clients_copy(clients_);
  for (ClientList::const_iterator other_client = clients_copy.begin();
       other_client != clients_copy.end(); ++other_client) {
    if ((*other_client) != client) {
      (*other_client)->Disconnect();
    }
  }

  // Create a new RecordSession if there was none.
  if (!recorder_.get()) {
    // Then we create a ScreenRecorder passing the message loops that
    // it should run on.
    Encoder* encoder = CreateEncoder(client->connection()->session()->config());

    recorder_ = new ScreenRecorder(context_->main_message_loop(),
                                   context_->encode_message_loop(),
                                   context_->network_message_loop(),
                                   desktop_environment_->capturer(),
                                   encoder);
  }

  // Immediately add the connection and start the session.
  recorder_->AddConnection(client->connection());
  recorder_->Start();

  // Notify observers that there is at least one authenticated client.
  const std::string& jid = client->connection()->session()->jid();
  for (StatusObserverList::iterator it = status_observers_.begin();
       it != status_observers_.end(); ++it) {
    (*it)->OnClientAuthenticated(jid);
  }
  // TODO(jamiewalch): Tidy up actions to be taken on connect/disconnect,
  // including closing the connection on failure of a critical operation.
  EnableCurtainMode(true);

  std::string username = jid.substr(0, jid.find('/'));
  desktop_environment_->OnConnect(username);
 }
