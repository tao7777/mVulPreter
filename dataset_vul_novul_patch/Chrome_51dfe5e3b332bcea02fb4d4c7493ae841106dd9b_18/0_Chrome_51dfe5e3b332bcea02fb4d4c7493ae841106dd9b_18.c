bool PulseAudioMixer::Init(InitDoneCallback* callback) {
void AudioMixerPulse::Init(InitDoneCallback* callback) {
  DCHECK(callback);
  if (!InitThread()) {
    callback->Run(false);
    delete callback;
    return;
  }
 
   thread_->message_loop()->PostTask(FROM_HERE,
      NewRunnableMethod(this, &AudioMixerPulse::DoInit, callback));
 }
