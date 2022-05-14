bool PulseAudioMixer::InitSync() {
bool AudioMixerPulse::InitSync() {
   if (!InitThread())
     return false;
   return PulseAudioInit();
 }
