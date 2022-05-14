 void ChromotingHost::ShutdownFinish() {
  if (MessageLoop::current() != context_->main_message_loop()) {
    context_->main_message_loop()->PostTask(
        FROM_HERE, base::Bind(&ChromotingHost::ShutdownFinish, this));
    return;
  }
 
  {
    base::AutoLock auto_lock(lock_);
    state_ = kStopped;
  }
 
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
