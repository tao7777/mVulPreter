void AudioOutputController::DoFlush() {
  DCHECK_EQ(message_loop_, MessageLoop::current());


   if (!sync_reader_) {
     if (state_ != kPaused)
       return;
     buffer_.Clear();
   }
 }
