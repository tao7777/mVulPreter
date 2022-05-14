void PulseAudioMixer::DoGetVolume(GetVolumeCallback* callback,
                                  void* user) {
  callback->Run(GetVolumeDb(), user);
  delete callback;
}
