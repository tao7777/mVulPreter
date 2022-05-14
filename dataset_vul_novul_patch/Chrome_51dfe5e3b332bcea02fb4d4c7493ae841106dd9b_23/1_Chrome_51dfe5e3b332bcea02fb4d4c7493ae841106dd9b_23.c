inline bool PulseAudioMixer::MainloopLockIfReady() const {
   AutoLock lock(mixer_state_lock_);
   if (mixer_state_ != READY)
     return false;
  if (!pa_mainloop_)
    return false;
  pa_threaded_mainloop_lock(pa_mainloop_);
  ++mainloop_lock_count_;
  return true;
}
