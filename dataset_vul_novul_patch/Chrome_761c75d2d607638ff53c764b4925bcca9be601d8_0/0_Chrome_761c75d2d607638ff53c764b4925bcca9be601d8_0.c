void MediaElementAudioSourceHandler::OnCurrentSrcChanged(
bool MediaElementAudioSourceHandler::WouldTaintOrigin() {
  // If we're cross-origin and allowed access vie CORS, we're not tainted.
  if (MediaElement()->GetWebMediaPlayer()->DidPassCORSAccessCheck()) {
    return false;
  }
 
  // Handles the case where the url is a redirect to another site that we're not
  // allowed to access.
  if (!MediaElement()->HasSingleSecurityOrigin()) {
    return true;
  }
 
