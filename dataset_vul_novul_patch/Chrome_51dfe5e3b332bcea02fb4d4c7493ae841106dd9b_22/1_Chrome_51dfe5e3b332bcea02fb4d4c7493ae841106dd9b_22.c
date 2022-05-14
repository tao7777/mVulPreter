inline void PulseAudioMixer::MainloopLock() const {
   pa_threaded_mainloop_lock(pa_mainloop_);
   ++mainloop_lock_count_;
 }
