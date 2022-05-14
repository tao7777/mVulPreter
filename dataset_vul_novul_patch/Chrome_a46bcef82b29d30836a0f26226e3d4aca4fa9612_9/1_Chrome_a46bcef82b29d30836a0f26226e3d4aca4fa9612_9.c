 void ChromotingHost::PauseSession(bool pause) {
  if (context_->main_message_loop() != MessageLoop::current()) {
    context_->main_message_loop()->PostTask(
         FROM_HERE, base::Bind(&ChromotingHost::PauseSession, this, pause));
     return;
   }
   ClientList::iterator client;
   for (client = clients_.begin(); client != clients_.end(); ++client) {
     client->get()->set_awaiting_continue_approval(pause);
  }
  desktop_environment_->OnPause(pause);
}
