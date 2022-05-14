bool MediaElementAudioSourceHandler::PassesCORSAccessCheck() {
  DCHECK(MediaElement());
  return (MediaElement()->GetWebMediaPlayer() &&
          MediaElement()->GetWebMediaPlayer()->DidPassCORSAccessCheck()) ||
         passes_current_src_cors_access_check_;
}
