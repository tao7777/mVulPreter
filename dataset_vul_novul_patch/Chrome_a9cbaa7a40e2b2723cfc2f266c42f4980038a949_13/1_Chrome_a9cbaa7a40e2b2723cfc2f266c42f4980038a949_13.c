 bool MediaElementAudioSourceHandler::WouldTaintOrigin() {
  if (MediaElement()->GetWebMediaPlayer()->DidPassCORSAccessCheck()) {
    return false;
  }
  if (!MediaElement()->HasSingleSecurityOrigin()) {
    return true;
  }
  return Context()->WouldTaintOrigin(MediaElement()->currentSrc());
 }
