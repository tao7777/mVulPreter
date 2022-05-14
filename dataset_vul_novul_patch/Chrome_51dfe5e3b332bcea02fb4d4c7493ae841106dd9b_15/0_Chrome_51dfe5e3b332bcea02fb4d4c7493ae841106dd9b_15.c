void PulseAudioMixer::GetDefaultPlaybackDevice() {
void AudioMixerPulse::GetDefaultPlaybackDevice() {
   DCHECK_GT(mainloop_lock_count_, 0);
   DCHECK(pa_context_);
   DCHECK(pa_context_get_state(pa_context_) == PA_CONTEXT_READY);

  CallbackWrapper data = {this, false, NULL};

  pa_operation* pa_op = pa_context_get_sink_info_list(pa_context_,
                                        EnumerateDevicesCallback,
                                        &data);
  CompleteOperation(pa_op, &data.done);
   return;
 }
