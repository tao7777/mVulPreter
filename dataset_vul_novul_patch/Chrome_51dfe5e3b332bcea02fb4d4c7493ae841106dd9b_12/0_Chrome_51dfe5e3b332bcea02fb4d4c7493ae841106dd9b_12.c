void PulseAudioMixer::EnumerateDevicesCallback(pa_context* unused,
void AudioMixerPulse::EnumerateDevicesCallback(pa_context* unused,
                                                const pa_sink_info* sink_info,
                                                int eol,
                                                void* userdata) {
  CallbackWrapper* data =
      static_cast<CallbackWrapper*>(userdata);
  data->instance->OnEnumerateDevices(sink_info, eol, &data->done);
 }
