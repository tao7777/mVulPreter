bool ExtensionTtsPlatformImplChromeOs::IsSpeaking() {
bool ExtensionTtsPlatformImplChromeOs::SendsEvent(TtsEventType event_type) {
  return (event_type == TTS_EVENT_START ||
          event_type == TTS_EVENT_END ||
          event_type == TTS_EVENT_ERROR);
}

void ExtensionTtsPlatformImplChromeOs::PollUntilSpeechFinishes(
    int utterance_id) {
  if (utterance_id != utterance_id_) {
    // This utterance must have been interrupted or cancelled.
    return;
   }
 
  chromeos::CrosLibrary* cros_library = chromeos::CrosLibrary::Get();
  ExtensionTtsController* controller = ExtensionTtsController::GetInstance();

  if (!cros_library->EnsureLoaded()) {
    controller->OnTtsEvent(
        utterance_id_, TTS_EVENT_ERROR, 0, kCrosLibraryNotLoadedError);
    return;
  }

  if (!cros_library->GetSpeechSynthesisLibrary()->IsSpeaking()) {
    controller->OnTtsEvent(
        utterance_id_, TTS_EVENT_END, utterance_length_, std::string());
    return;
  }

  MessageLoop::current()->PostDelayedTask(
      FROM_HERE, method_factory_.NewRunnableMethod(
          &ExtensionTtsPlatformImplChromeOs::PollUntilSpeechFinishes,
          utterance_id),
      kSpeechCheckDelayIntervalMs);
 }
