 void ChromotingHost::StopScreenRecorder() {
  DCHECK(context_->network_message_loop()->BelongsToCurrentThread());
   DCHECK(recorder_.get());
 
   ++stopping_recorders_;
  scoped_refptr<ScreenRecorder> recorder = recorder_;
   recorder_ = NULL;
  recorder->Stop(base::Bind(&ChromotingHost::OnScreenRecorderStopped, this));
 }
