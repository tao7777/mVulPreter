bool ExtensionTtsController::IsSpeaking() const {
  return current_utterance_ != NULL;
}
