void PulseAudioMixer::OnConnectToPulseCallback(
     pa_context* context, bool* connect_done) {
   pa_context_state_t state = pa_context_get_state(context);
   if (state == PA_CONTEXT_READY ||
      state == PA_CONTEXT_FAILED ||
      state == PA_CONTEXT_TERMINATED) {
    *connect_done = true;
    MainloopSignal();
   }
 }
