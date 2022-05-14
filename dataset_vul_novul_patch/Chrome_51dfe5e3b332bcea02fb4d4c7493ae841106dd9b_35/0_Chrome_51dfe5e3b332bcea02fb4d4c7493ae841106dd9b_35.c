PulseAudioMixer::~PulseAudioMixer() {
AudioMixerPulse::~AudioMixerPulse() {
   PulseAudioFree();
  if (thread_ != NULL) {
    thread_->Stop();
    thread_.reset();
  }
 }
