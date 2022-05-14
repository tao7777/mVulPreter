bool PulseAudioMixer::InitSync() {
   if (!InitThread())
     return false;
   return PulseAudioInit();
 }
