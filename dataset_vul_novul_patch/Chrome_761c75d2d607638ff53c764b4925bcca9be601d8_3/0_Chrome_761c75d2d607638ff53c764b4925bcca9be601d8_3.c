bool MediaElementAudioSourceHandler::PassesCurrentSrcCORSAccessCheck(
  // Test to see if the current media URL taint the origin of the audio context?
  return Context()->WouldTaintOrigin(MediaElement()->currentSrc());
 }
