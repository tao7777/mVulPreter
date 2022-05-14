inline void PulseAudioMixer::MainloopUnlock() const {
inline void AudioMixerPulse::MainloopUnlock() const {
   --mainloop_lock_count_;
   pa_threaded_mainloop_unlock(pa_mainloop_);
 }
