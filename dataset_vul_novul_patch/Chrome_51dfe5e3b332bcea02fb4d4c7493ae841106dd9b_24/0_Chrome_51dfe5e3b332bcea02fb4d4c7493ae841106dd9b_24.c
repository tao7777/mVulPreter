inline bool PulseAudioMixer::MainloopSafeLock() const {
inline bool AudioMixerPulse::MainloopSafeLock() const {
   AutoLock lock(mixer_state_lock_);
   if ((mixer_state_ == SHUTTING_DOWN) || (!pa_mainloop_))
     return false;

   pa_threaded_mainloop_lock(pa_mainloop_);
   ++mainloop_lock_count_;
   return true;
 }
