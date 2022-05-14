 bool ExtensionTtsPlatformImplChromeOs::Speak(
    int utterance_id,
     const std::string& utterance,
    const std::string& lang,
    const UtteranceContinuousParameters& params) {
   chromeos::CrosLibrary* cros_library = chromeos::CrosLibrary::Get();
   if (!cros_library->EnsureLoaded()) {
     set_error(kCrosLibraryNotLoadedError);
     return false;
   }
 
  utterance_id_ = utterance_id;
  utterance_length_ = utterance.size();

   std::string options;
 
  if (!lang.empty()) {
     AppendSpeakOption(
         chromeos::SpeechSynthesisLibrary::kSpeechPropertyLocale,
        lang,
         &options);
   }
 
  if (params.rate >= 0.0) {
     AppendSpeakOption(
         chromeos::SpeechSynthesisLibrary::kSpeechPropertyRate,
        DoubleToString(1.5 + params.rate * 2.5),
         &options);
   }
 
  if (params.pitch >= 0.0) {
     AppendSpeakOption(
         chromeos::SpeechSynthesisLibrary::kSpeechPropertyPitch,
        DoubleToString(params.pitch),
         &options);
   }
 
  if (params.volume >= 0.0) {
     AppendSpeakOption(
         chromeos::SpeechSynthesisLibrary::kSpeechPropertyVolume,
        DoubleToString(params.volume * 5),
         &options);
   }
 
  if (!options.empty()) {
    cros_library->GetSpeechSynthesisLibrary()->SetSpeakProperties(
         options.c_str());
   }
 
  bool result =
      cros_library->GetSpeechSynthesisLibrary()->Speak(utterance.c_str());

  if (result) {
    ExtensionTtsController* controller = ExtensionTtsController::GetInstance();
    controller->OnTtsEvent(utterance_id_, TTS_EVENT_START, 0, std::string());
    PollUntilSpeechFinishes(utterance_id_);
  }

  return result;
 }
