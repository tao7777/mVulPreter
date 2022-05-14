bool PulseAudioMixer::IsMute() const {
bool AudioMixerPulse::IsMute() const {
   if (!MainloopLockIfReady())
     return false;
   AudioInfo data;
  GetAudioInfo(&data);
  MainloopUnlock();
   return data.muted;
 }
