 AudioOutputAuthorizationHandler::AudioOutputAuthorizationHandler(
    media::AudioSystem* audio_system,
     MediaStreamManager* media_stream_manager,
     int render_process_id,
     const std::string& salt)
    : audio_system_(audio_system),
       media_stream_manager_(media_stream_manager),
       permission_checker_(base::MakeUnique<MediaDevicesPermissionChecker>()),
       render_process_id_(render_process_id),
      salt_(salt),
      weak_factory_(this) {
  DCHECK(media_stream_manager_);
}
