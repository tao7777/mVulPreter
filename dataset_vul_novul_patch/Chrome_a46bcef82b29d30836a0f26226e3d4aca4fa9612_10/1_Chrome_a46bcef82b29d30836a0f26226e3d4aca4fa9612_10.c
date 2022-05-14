 void ChromotingHost::Shutdown(const base::Closure& shutdown_task) {
  if (MessageLoop::current() != context_->main_message_loop()) {
    context_->main_message_loop()->PostTask(
        FROM_HERE,
        base::Bind(&ChromotingHost::Shutdown, this, shutdown_task));
     return;
   }
 
  {
    base::AutoLock auto_lock(lock_);
    if (state_ == kInitial || state_ == kStopped) {
      state_ = kStopped;
      context_->main_message_loop()->PostTask(FROM_HERE, shutdown_task);
      return;
    }
    if (!shutdown_task.is_null())
      shutdown_tasks_.push_back(shutdown_task);
    if (state_ == kStopping)
      return;
    state_ = kStopping;
   }
 
   while (!clients_.empty()) {
    scoped_refptr<ClientSession> client = clients_.front();
    client->Disconnect();
    OnClientDisconnected(client);
   }
 
  ShutdownNetwork();
 }
