bool PulseAudioMixer::Init(InitDoneCallback* callback) {
  if (!InitThread())
    return false;
 
   thread_->message_loop()->PostTask(FROM_HERE,
    NewRunnableMethod(this, &PulseAudioMixer::DoInit, callback));
  return true;
 }
