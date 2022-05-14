void PulseAudioMixer::ConnectToPulseCallbackThunk(
void AudioMixerPulse::ConnectToPulseCallbackThunk(
     pa_context* context, void* userdata) {
   CallbackWrapper* data =
       static_cast<CallbackWrapper*>(userdata);
   data->instance->OnConnectToPulseCallback(context, &data->done);
 }
