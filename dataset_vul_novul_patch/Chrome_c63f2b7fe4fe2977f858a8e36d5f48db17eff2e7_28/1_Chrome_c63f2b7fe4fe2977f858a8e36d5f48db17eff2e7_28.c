 ExtensionTtsPlatformImplWin::ExtensionTtsPlatformImplWin()
   : speech_synthesizer_(NULL),
     paused_(false) {
  CoCreateInstance(
      CLSID_SpVoice,
      NULL,
       CLSCTX_SERVER,
       IID_ISpVoice,
       reinterpret_cast<void**>(&speech_synthesizer_));
 }
