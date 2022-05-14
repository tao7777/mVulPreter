void PulseAudioMixer::DoInit(InitDoneCallback* callback) {
void AudioMixerPulse::DoInit(InitDoneCallback* callback) {
   bool success = PulseAudioInit();
   callback->Run(success);
   delete callback;
 }
