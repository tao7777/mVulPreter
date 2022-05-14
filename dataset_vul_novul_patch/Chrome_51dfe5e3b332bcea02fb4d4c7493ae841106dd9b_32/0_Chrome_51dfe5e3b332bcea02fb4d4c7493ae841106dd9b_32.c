PulseAudioMixer::PulseAudioMixer()
AudioMixerPulse::AudioMixerPulse()
     : device_id_(kInvalidDeviceId),
       last_channels_(0),
       mainloop_lock_count_(0),
       mixer_state_(UNINITIALIZED),
       pa_context_(NULL),
      pa_mainloop_(NULL) {
 }
