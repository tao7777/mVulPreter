void ExtensionTtsController::FinishCurrentUtterance() {
  if (current_utterance_) {
    current_utterance_->FinishAndDestroy();
    current_utterance_ = NULL;
   }
}
