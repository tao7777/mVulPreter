 MediaRecorderHandler::~MediaRecorderHandler() {
   DCHECK(main_render_thread_checker_.CalledOnValidThread());
  if (client_) {
     client_->WriteData(
         nullptr, 0u, true,
         (TimeTicks::Now() - TimeTicks::UnixEpoch()).InMillisecondsF());
  }
 }
