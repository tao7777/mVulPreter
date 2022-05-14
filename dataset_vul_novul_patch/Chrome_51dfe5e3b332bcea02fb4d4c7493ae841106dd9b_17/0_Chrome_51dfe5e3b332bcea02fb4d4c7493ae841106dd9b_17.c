bool PulseAudioMixer::GetVolumeDbAsync(GetVolumeCallback* callback,
bool AudioMixerPulse::GetVolumeLimits(double* vol_min, double* vol_max) {
  if (vol_min)
    *vol_min = kMinVolumeDb;
  if (vol_max)
    *vol_max = kMaxVolumeDb;
   return true;
 }
