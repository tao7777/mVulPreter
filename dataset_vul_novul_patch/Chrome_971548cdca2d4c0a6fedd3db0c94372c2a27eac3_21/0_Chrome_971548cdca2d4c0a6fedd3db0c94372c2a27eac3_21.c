SpeechRecognitionManagerImpl::SpeechRecognitionManagerImpl(
    media::AudioSystem* audio_system,
    MediaStreamManager* media_stream_manager)
    : audio_system_(audio_system),
      media_stream_manager_(media_stream_manager),
      primary_session_id_(kSessionIDInvalid),
      last_session_id_(kSessionIDInvalid),
      is_dispatching_event_(false),
       delegate_(GetContentClient()
                     ->browser()
                     ->CreateSpeechRecognitionManagerDelegate()),
      requester_id_(next_requester_id_++),
       weak_factory_(this) {
   DCHECK(!g_speech_recognition_manager_impl);
   g_speech_recognition_manager_impl = this;

  frame_deletion_observer_.reset(new FrameDeletionObserver(
      base::BindRepeating(&SpeechRecognitionManagerImpl::AbortSessionImpl,
                          weak_factory_.GetWeakPtr())));
}
