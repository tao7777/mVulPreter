void ExtensionTtsController::ClearUtteranceQueue() {
  while (!utterance_queue_.empty()) {
    Utterance* utterance = utterance_queue_.front();
    utterance_queue_.pop();
    utterance->set_error(kSpeechRemovedFromQueueError);
    utterance->FinishAndDestroy();
   }
}
