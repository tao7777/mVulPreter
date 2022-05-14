 ExtensionTtsPlatformImplWin::ExtensionTtsPlatformImplWin()
   : speech_synthesizer_(NULL),
     paused_(false) {
  CoCreateInstance(
      CLSID_SpVoice,
      NULL,
       CLSCTX_SERVER,
       IID_ISpVoice,
       reinterpret_cast<void**>(&speech_synthesizer_));
  if (speech_synthesizer_) {
    ULONGLONG event_mask =
        SPFEI(SPEI_START_INPUT_STREAM) |
        SPFEI(SPEI_TTS_BOOKMARK) |
        SPFEI(SPEI_WORD_BOUNDARY) |
        SPFEI(SPEI_SENTENCE_BOUNDARY) |
        SPFEI(SPEI_END_INPUT_STREAM);
    speech_synthesizer_->SetInterest(event_mask, event_mask);
    speech_synthesizer_->SetNotifyCallbackFunction(
        ExtensionTtsPlatformImplWin::SpeechEventCallback, 0, 0);
  }
 }
