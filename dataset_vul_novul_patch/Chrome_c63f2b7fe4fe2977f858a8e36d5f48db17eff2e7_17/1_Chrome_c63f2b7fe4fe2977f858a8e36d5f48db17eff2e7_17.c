void ExtensionTtsSpeakFunction::SpeechFinished() {
  error_ = utterance_->error();
  bool success = error_.empty();
  SendResponse(success);
  Release();  // Balanced in RunImpl().
}
