 void ChromotingHost::StopScreenRecorder() {
  DCHECK(MessageLoop::current() == context_->main_message_loop());
   DCHECK(recorder_.get());
 
   ++stopping_recorders_;
  recorder_->Stop(base::Bind(&ChromotingHost::OnScreenRecorderStopped, this));
   recorder_ = NULL;
 }
