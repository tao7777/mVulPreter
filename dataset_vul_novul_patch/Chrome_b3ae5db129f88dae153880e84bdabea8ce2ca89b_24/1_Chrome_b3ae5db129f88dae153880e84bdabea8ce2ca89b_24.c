void CrosLibrary::TestApi::SetSpeechSynthesisLibrary(
    SpeechSynthesisLibrary* library, bool own) {
  library_->speech_synthesis_lib_.SetImpl(library, own);
}
