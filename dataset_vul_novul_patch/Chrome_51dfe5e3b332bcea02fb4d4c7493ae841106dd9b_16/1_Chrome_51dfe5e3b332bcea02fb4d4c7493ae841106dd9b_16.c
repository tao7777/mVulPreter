double PulseAudioMixer::GetVolumeDb() const {
   if (!MainloopLockIfReady())
    return pa_sw_volume_to_dB(0);  // this returns -inf.
   AudioInfo data;
   GetAudioInfo(&data);
   MainloopUnlock();
   return pa_sw_volume_to_dB(data.cvolume.values[0]);
 }
