inline void PulseAudioMixer::MainloopUnlock() const {
   --mainloop_lock_count_;
   pa_threaded_mainloop_unlock(pa_mainloop_);
 }
