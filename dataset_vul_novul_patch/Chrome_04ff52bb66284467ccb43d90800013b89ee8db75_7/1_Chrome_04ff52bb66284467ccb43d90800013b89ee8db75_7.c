AudioParameters GetInputParametersOnDeviceThread(AudioManager* audio_manager,
                                                 const std::string& device_id) {
   DCHECK(audio_manager->GetTaskRunner()->BelongsToCurrentThread());
 
   if (!audio_manager->HasAudioInputDevices())
     return AudioParameters();
 
   return audio_manager->GetInputStreamParameters(device_id);
 }
