void ChromotingHost::ShutdownNetwork() {
  if (!context_->network_message_loop()->BelongsToCurrentThread()) {
    context_->network_message_loop()->PostTask(
        FROM_HERE, base::Bind(&ChromotingHost::ShutdownNetwork, this));
    return;
  }
  if (session_manager_.get()) {
    session_manager_->Close();
    session_manager_.reset();
  }
  if (signal_strategy_.get()) {
    signal_strategy_->Close();
    signal_strategy_.reset();
    for (StatusObserverList::iterator it = status_observers_.begin();
         it != status_observers_.end(); ++it) {
      (*it)->OnSignallingDisconnected();
    }
  }
  ShutdownRecorder();
}
