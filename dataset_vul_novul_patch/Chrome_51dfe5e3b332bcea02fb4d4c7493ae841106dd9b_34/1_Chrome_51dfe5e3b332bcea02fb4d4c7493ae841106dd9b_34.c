void PulseAudioMixer::SetVolumeDb(double vol_db) {
   if (!MainloopLockIfReady())
     return;
 
  if (!last_channels_) {
    AudioInfo data;
    GetAudioInfo(&data);
    last_channels_ = data.cvolume.channels;
  }

  pa_operation* pa_op;
  pa_cvolume cvolume;
  pa_cvolume_set(&cvolume, last_channels_, pa_sw_volume_from_dB(vol_db));
  pa_op = pa_context_set_sink_volume_by_index(pa_context_, device_id_,
                                               &cvolume, NULL, NULL);
   pa_operation_unref(pa_op);
   MainloopUnlock();
 }
