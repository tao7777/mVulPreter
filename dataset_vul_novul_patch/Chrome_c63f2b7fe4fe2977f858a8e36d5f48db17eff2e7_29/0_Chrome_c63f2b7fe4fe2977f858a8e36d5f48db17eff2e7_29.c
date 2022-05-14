 bool ExtensionTtsPlatformImplWin::Speak(
    int utterance_id,
     const std::string& src_utterance,
    const std::string& lang,
    const UtteranceContinuousParameters& params) {
  std::wstring prefix;
  std::wstring suffix;
 
   if (!speech_synthesizer_)
     return false;
 
  // TODO(dmazzoni): support languages other than the default: crbug.com/88059
 
  if (params.rate >= 0.0) {
    // Map our multiplicative range of 0.1x to 10.0x onto Microsoft's
    // linear range of -10 to 10:
    //   0.1 -> -10
    //   1.0 -> 0
    //  10.0 -> 10
    speech_synthesizer_->SetRate(static_cast<int32>(10 * log10(params.rate)));
   }
 
  if (params.pitch >= 0.0) {
     std::wstring pitch_value =
        base::IntToString16(static_cast<int>(params.pitch * 10 - 10));
    prefix = L"<pitch absmiddle=\"" + pitch_value + L"\">";
    suffix = L"</pitch>";
   }
 
  if (params.volume >= 0.0) {
    speech_synthesizer_->SetVolume(static_cast<uint16>(params.volume * 100));
   }
 
   if (paused_) {
     speech_synthesizer_->Resume();
     paused_ = false;
   }
 
  // TODO(dmazzoni): convert SSML to SAPI xml. http://crbug.com/88072

  utterance_ = UTF8ToWide(src_utterance);
  utterance_id_ = utterance_id;
  char_position_ = 0;
  std::wstring merged_utterance = prefix + utterance_ + suffix;
  prefix_len_ = prefix.size();


  HRESULT result = speech_synthesizer_->Speak(
      merged_utterance.c_str(),
      SPF_ASYNC | SPF_PURGEBEFORESPEAK,
      &stream_number_);
  return (result == S_OK);
 }
