 void ChromotingHost::Shutdown(const base::Closure& shutdown_task) {
  if (!context_->network_message_loop()->BelongsToCurrentThread()) {
    context_->network_message_loop()->PostTask(
        FROM_HERE, base::Bind(&ChromotingHost::Shutdown, this, shutdown_task));
     return;
   }
 
  if (state_ == kInitial || state_ == kStopped) {
    state_ = kStopped;
    context_->network_message_loop()->PostTask(FROM_HERE, shutdown_task);
    return;
   }
  if (!shutdown_task.is_null())
    shutdown_tasks_.push_back(shutdown_task);
  if (state_ == kStopping)
    return;
  state_ = kStopping;
 
   while (!clients_.empty()) {
    clients_.front()->Disconnect();
  }

  // Stop session manager.
  if (session_manager_.get()) {
    session_manager_->Close();
    // It may not be safe to delete |session_manager_| here becase
    // this method may be invoked in response to a libjingle event and
    // libjingle's sigslot doesn't handle it properly, so postpone the
    // deletion.
    context_->network_message_loop()->DeleteSoon(
        FROM_HERE, session_manager_.release());
   }
 
  // Stop XMPP connection synchronously.
  if (signal_strategy_.get()) {
    signal_strategy_->Close();
    signal_strategy_.reset();

    for (StatusObserverList::iterator it = status_observers_.begin();
         it != status_observers_.end(); ++it) {
      (*it)->OnSignallingDisconnected();
    }
  }

  if (recorder_.get()) {
    StopScreenRecorder();
  } else {
    ShutdownFinish();
  }
 }
