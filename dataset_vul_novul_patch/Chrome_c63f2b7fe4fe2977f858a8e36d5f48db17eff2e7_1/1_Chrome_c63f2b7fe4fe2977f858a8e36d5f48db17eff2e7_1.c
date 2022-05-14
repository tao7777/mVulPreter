void ExtensionTtsController::CheckSpeechStatus() {
  if (!current_utterance_)
    return;
 
  if (!current_utterance_->extension_id().empty())
    return;
  if (GetPlatformImpl()->IsSpeaking() == false) {
    FinishCurrentUtterance();
    SpeakNextUtterance();
   }
 
  if (current_utterance_ && current_utterance_->extension_id().empty()) {
    MessageLoop::current()->PostDelayedTask(
        FROM_HERE, method_factory_.NewRunnableMethod(
            &ExtensionTtsController::CheckSpeechStatus),
        kSpeechCheckDelayIntervalMs);
   }
}
