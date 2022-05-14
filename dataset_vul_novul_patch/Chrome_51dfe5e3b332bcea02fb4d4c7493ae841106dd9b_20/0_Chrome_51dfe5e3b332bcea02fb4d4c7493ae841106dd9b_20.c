bool PulseAudioMixer::InitThread() {
bool AudioMixerPulse::InitThread() {
   AutoLock lock(mixer_state_lock_);
 
   if (mixer_state_ != UNINITIALIZED)
     return false;

   if (thread_ == NULL) {
    thread_.reset(new base::Thread("AudioMixerPulse"));
     if (!thread_->Start()) {
       thread_.reset();
       return false;
    }
  }
  mixer_state_ = INITIALIZING;
  return true;
 }
