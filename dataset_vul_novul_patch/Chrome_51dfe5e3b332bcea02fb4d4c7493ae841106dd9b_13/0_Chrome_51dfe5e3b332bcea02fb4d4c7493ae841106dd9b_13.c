void PulseAudioMixer::GetAudioInfo(AudioInfo* info) const {
void AudioMixerPulse::GetAudioInfo(AudioInfo* info) const {
   DCHECK_GT(mainloop_lock_count_, 0);
  CallbackWrapper data = {const_cast<AudioMixerPulse*>(this), false, info};
   pa_operation* pa_op = pa_context_get_sink_info_by_index(pa_context_,
                                                           device_id_,
                                                           GetAudioInfoCallback,
                                                          &data);
  CompleteOperation(pa_op, &data.done);
 }
