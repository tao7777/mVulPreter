void PulseAudioMixer::OnEnumerateDevices(const pa_sink_info* sink_info,
void AudioMixerPulse::OnEnumerateDevices(const pa_sink_info* sink_info,
                                          int eol, bool* done) {
   if (device_id_ != kInvalidDeviceId)
     return;


  if (eol == 0) {
    device_id_ = sink_info->index;
  }
  *done = true;
  MainloopSignal();
 }
