void ExtensionTtsController::OnSpeechFinished(
    int request_id, const std::string& error_message) {
  if (!current_utterance_ || request_id != current_utterance_->id())
    return;
  current_utterance_->set_error(error_message);
  FinishCurrentUtterance();
  SpeakNextUtterance();
}
