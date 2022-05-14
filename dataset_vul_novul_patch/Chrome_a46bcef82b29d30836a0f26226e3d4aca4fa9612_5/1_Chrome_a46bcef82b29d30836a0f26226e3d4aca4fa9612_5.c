 void ChromotingHost::OnScreenRecorderStopped() {
  if (MessageLoop::current() != context_->main_message_loop()) {
    context_->main_message_loop()->PostTask(
         FROM_HERE, base::Bind(&ChromotingHost::OnScreenRecorderStopped, this));
     return;
   }
 
   --stopping_recorders_;
   DCHECK_GE(stopping_recorders_, 0);
 
  bool stopping;
  {
    base::AutoLock auto_lock(lock_);
    stopping = state_ == kStopping;
  }
  if (!stopping_recorders_ && stopping)
     ShutdownFinish();
 }
