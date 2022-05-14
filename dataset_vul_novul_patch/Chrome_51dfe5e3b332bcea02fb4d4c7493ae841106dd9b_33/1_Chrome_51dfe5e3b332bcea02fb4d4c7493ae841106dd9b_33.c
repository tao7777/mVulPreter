void PulseAudioMixer::SetMute(bool mute) {
   if (!MainloopLockIfReady())
     return;
   pa_operation* pa_op;
   pa_op = pa_context_set_sink_mute_by_index(pa_context_, device_id_,
                                             mute ? 1 : 0, NULL, NULL);
   pa_operation_unref(pa_op);
   MainloopUnlock();
 }
