PulseAudioMixer::~PulseAudioMixer() {
   PulseAudioFree();
  thread_->Stop();
  thread_.reset();
 }
