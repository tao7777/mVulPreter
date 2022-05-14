inline void PulseAudioMixer::MainloopSignal() const {
inline void AudioMixerPulse::MainloopSignal() const {
   DCHECK_GT(mainloop_lock_count_, 0);
   pa_threaded_mainloop_signal(pa_mainloop_, 0);
 }
