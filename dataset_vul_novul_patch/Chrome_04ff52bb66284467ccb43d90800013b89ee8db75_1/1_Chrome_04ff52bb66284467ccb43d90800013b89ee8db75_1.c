media::AudioParameters GetDeviceParametersOnDeviceThread(
    media::AudioManager* audio_manager,
    const std::string& unique_id) {
  DCHECK(audio_manager->GetTaskRunner()->BelongsToCurrentThread());
  return media::AudioDeviceDescription::IsDefaultDevice(unique_id)
             ? audio_manager->GetDefaultOutputStreamParameters()
             : audio_manager->GetOutputStreamParameters(unique_id);
}
