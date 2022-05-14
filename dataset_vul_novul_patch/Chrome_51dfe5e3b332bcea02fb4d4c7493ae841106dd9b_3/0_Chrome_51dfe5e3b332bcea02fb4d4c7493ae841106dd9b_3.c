 void AudioHandler::SetMute(bool do_mute) {
   if (!VerifyMixerConnection())
     return;
   DVLOG(1) << "Setting Mute to " << do_mute;
   mixer_->SetMute(do_mute);
 }
