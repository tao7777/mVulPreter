void ChromotingHost::OnClientDisconnected(ClientSession* client) {
  DCHECK_EQ(context_->main_message_loop(), MessageLoop::current());
  scoped_refptr<ClientSession> client_ref = client;
  ClientList::iterator it;
  for (it = clients_.begin(); it != clients_.end(); ++it) {
    if (it->get() == client)
      break;
  }
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
      StopScreenRecorder();
    }
    EnableCurtainMode(false);
    desktop_environment_->OnLastDisconnect();
  }
}
