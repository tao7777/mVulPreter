void ChromotingHost::ShutdownRecorder() {
  if (MessageLoop::current() != context_->main_message_loop()) {
    context_->main_message_loop()->PostTask(
        FROM_HERE, base::Bind(&ChromotingHost::ShutdownRecorder, this));
    return;
  }
  if (recorder_.get()) {
    StopScreenRecorder();
  } else if (!stopping_recorders_) {
    ShutdownFinish();
  }
}
