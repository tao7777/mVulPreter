void PulseAudioMixer::DoInit(InitDoneCallback* callback) {
   bool success = PulseAudioInit();
   callback->Run(success);
   delete callback;
 }
