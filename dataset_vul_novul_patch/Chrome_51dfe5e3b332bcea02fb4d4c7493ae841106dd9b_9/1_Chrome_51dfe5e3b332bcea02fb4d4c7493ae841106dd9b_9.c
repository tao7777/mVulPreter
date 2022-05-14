void PulseAudioMixer::ConnectToPulseCallbackThunk(
     pa_context* context, void* userdata) {
   CallbackWrapper* data =
       static_cast<CallbackWrapper*>(userdata);
   data->instance->OnConnectToPulseCallback(context, &data->done);
 }
