 void AudioOutputDevice::Stop() {
   {
     base::AutoLock auto_lock(audio_thread_lock_);
    audio_thread_.Stop(MessageLoop::current());
   }
 
   message_loop()->PostTask(FROM_HERE,
      base::Bind(&AudioOutputDevice::ShutDownOnIOThread, this));
}
