 bool ExtensionTtsPlatformImplChromeOs::Speak(
     const std::string& utterance,
    const std::string& locale,
    const std::string& gender,
    double rate,
    double pitch,
    double volume) {
   chromeos::CrosLibrary* cros_library = chromeos::CrosLibrary::Get();
   if (!cros_library->EnsureLoaded()) {
     set_error(kCrosLibraryNotLoadedError);
     return false;
   }
 
   std::string options;
 
  if (!locale.empty()) {
     AppendSpeakOption(
         chromeos::SpeechSynthesisLibrary::kSpeechPropertyLocale,
        locale,
         &options);
   }
 
  if (!gender.empty()) {
    AppendSpeakOption(
        chromeos::SpeechSynthesisLibrary::kSpeechPropertyGender,
        gender,
        &options);
  }
  if (rate >= 0.0) {
     AppendSpeakOption(
         chromeos::SpeechSynthesisLibrary::kSpeechPropertyRate,
        DoubleToString(rate * 5),
         &options);
   }
 
  if (pitch >= 0.0) {
     AppendSpeakOption(
         chromeos::SpeechSynthesisLibrary::kSpeechPropertyPitch,
        DoubleToString(pitch * 2),
         &options);
   }
 
  if (volume >= 0.0) {
     AppendSpeakOption(
         chromeos::SpeechSynthesisLibrary::kSpeechPropertyVolume,
        DoubleToString(volume * 5),
         &options);
   }
 
  if (!options.empty()) {
    cros_library->GetSpeechSynthesisLibrary()->SetSpeakProperties(
         options.c_str());
   }
 
  return cros_library->GetSpeechSynthesisLibrary()->Speak(utterance.c_str());
 }
