AudioParameters GetInputParametersOnDeviceThread(AudioManager* audio_manager,
                                                 const std::string& device_id) {
   DCHECK(audio_manager->GetTaskRunner()->BelongsToCurrentThread());
 
  // returns invalid parameters if the device is not found.
   if (!audio_manager->HasAudioInputDevices())
     return AudioParameters();
 
   return audio_manager->GetInputStreamParameters(device_id);
 }
