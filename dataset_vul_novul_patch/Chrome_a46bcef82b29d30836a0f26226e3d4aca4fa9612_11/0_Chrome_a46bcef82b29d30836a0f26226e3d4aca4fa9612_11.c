 void ChromotingHost::ShutdownFinish() {
  DCHECK(context_->network_message_loop()->BelongsToCurrentThread());
 
  state_ = kStopped;
 
  scoped_refptr<ChromotingHost> self(this);

  for (StatusObserverList::iterator it = status_observers_.begin();
       it != status_observers_.end(); ++it) {
    (*it)->OnShutdown();
  }

  for (std::vector<base::Closure>::iterator it = shutdown_tasks_.begin();
       it != shutdown_tasks_.end(); ++it) {
    it->Run();
  }
  shutdown_tasks_.clear();
}
