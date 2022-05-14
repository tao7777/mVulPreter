double PulseAudioMixer::GetVolumeDb() const {
double AudioMixerPulse::GetVolumeDb() const {
   if (!MainloopLockIfReady())
    return AudioMixer::kSilenceDb;
   AudioInfo data;
   GetAudioInfo(&data);
   MainloopUnlock();
   return pa_sw_volume_to_dB(data.cvolume.values[0]);
 }
