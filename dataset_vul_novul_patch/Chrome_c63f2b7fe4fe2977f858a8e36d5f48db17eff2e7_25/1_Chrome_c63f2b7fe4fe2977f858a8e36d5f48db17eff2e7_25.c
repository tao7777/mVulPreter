bool ExtensionTtsPlatformImplChromeOs::IsSpeaking() {
  if (chromeos::CrosLibrary::Get()->EnsureLoaded()) {
    return chromeos::CrosLibrary::Get()->GetSpeechSynthesisLibrary()->
        IsSpeaking();
   }
 
  set_error(kCrosLibraryNotLoadedError);
  return false;
 }
