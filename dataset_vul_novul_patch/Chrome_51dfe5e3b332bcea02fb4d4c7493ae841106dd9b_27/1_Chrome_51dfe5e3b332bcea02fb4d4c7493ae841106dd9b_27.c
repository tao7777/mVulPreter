inline void PulseAudioMixer::MainloopWait() const {
   DCHECK_GT(mainloop_lock_count_, 0);
   pa_threaded_mainloop_wait(pa_mainloop_);
 }
