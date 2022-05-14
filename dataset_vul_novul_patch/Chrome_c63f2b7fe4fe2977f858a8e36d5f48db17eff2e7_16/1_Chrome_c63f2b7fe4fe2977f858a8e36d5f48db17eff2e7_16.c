void ExtensionTtsController::SpeakOrEnqueue(Utterance* utterance) {
  if (IsSpeaking() && utterance->can_enqueue()) {
    utterance_queue_.push(utterance);
  } else {
    Stop();
    SpeakNow(utterance);
   }
}
