SpeechSynthesisLibrary* CrosLibrary::GetSpeechSynthesisLibrary() {
  return speech_synthesis_lib_.GetDefaultImpl(use_stub_impl_);
}
