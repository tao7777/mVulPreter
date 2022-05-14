PulseAudioMixer::State PulseAudioMixer::CheckState() const {
AudioMixer::State AudioMixerPulse::GetState() const {
   AutoLock lock(mixer_state_lock_);
   if ((mixer_state_ == READY) &&
      (pa_context_get_state(pa_context_) != PA_CONTEXT_READY))
    mixer_state_ = IN_ERROR;
  return mixer_state_;
}
