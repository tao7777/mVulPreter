void PulseAudioMixer::GetAudioInfoCallback(pa_context* unused,
void AudioMixerPulse::GetAudioInfoCallback(pa_context* unused,
                                            const pa_sink_info* sink_info,
                                            int eol,
                                            void* userdata) {
  CallbackWrapper* data = static_cast<CallbackWrapper*>(userdata);
  AudioInfo* info = static_cast<AudioInfo*>(data->userdata);

  if (eol == 0) {
    info->cvolume = sink_info->volume;
    info->muted = sink_info->mute ? true : false;
    data->done = true;
  }
   data->instance->MainloopSignal();
 }
