 void ChromotingHost::OnScreenRecorderStopped() {
  if (!context_->network_message_loop()->BelongsToCurrentThread()) {
    context_->network_message_loop()->PostTask(
         FROM_HERE, base::Bind(&ChromotingHost::OnScreenRecorderStopped, this));
     return;
   }
 
   --stopping_recorders_;
   DCHECK_GE(stopping_recorders_, 0);
 
  if (!stopping_recorders_ && state_ == kStopping)
     ShutdownFinish();
 }
