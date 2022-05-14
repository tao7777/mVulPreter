 void ChromotingHost::OnSessionClosed(ClientSession* client) {
   DCHECK(context_->network_message_loop()->BelongsToCurrentThread());
 
  scoped_refptr<ClientSession> client_ref = client;

  ClientList::iterator it = std::find(clients_.begin(), clients_.end(), client);
  CHECK(it != clients_.end());
  clients_.erase(it);

  if (recorder_.get()) {
    recorder_->RemoveConnection(client->connection());
  }

  for (StatusObserverList::iterator it = status_observers_.begin();
       it != status_observers_.end(); ++it) {
    (*it)->OnClientDisconnected(client->client_jid());
  }

  if (AuthenticatedClientsCount() == 0) {
    if (recorder_.get()) {
      // Stop the recorder if there are no more clients.
      StopScreenRecorder();
    }

    // Disable the "curtain" if there are no more active clients.
    EnableCurtainMode(false);
    desktop_environment_->OnLastDisconnect();
  }
 }
