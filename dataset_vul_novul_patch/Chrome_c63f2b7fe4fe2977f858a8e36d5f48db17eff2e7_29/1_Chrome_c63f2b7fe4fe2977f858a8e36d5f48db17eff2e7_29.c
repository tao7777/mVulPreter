 bool ExtensionTtsPlatformImplWin::Speak(
     const std::string& src_utterance,
    const std::string& language,
    const std::string& gender,
    double rate,
    double pitch,
    double volume) {
  std::wstring utterance = UTF8ToUTF16(src_utterance);
 
   if (!speech_synthesizer_)
     return false;
 
 
  if (rate >= 0.0) {
    speech_synthesizer_->SetRate(static_cast<int32>(rate * 20 - 10));
   }
 
  if (pitch >= 0.0) {
     std::wstring pitch_value =
        base::IntToString16(static_cast<int>(pitch * 20 - 10));
    utterance = L"<pitch absmiddle=\"" + pitch_value + L"\">" +
        utterance + L"</pitch>";
   }
 
  if (volume >= 0.0) {
    speech_synthesizer_->SetVolume(static_cast<uint16>(volume * 100));
   }
 
   if (paused_) {
     speech_synthesizer_->Resume();
     paused_ = false;
   }
  speech_synthesizer_->Speak(
      utterance.c_str(), SPF_ASYNC | SPF_PURGEBEFORESPEAK, NULL);
 
  return true;
 }
