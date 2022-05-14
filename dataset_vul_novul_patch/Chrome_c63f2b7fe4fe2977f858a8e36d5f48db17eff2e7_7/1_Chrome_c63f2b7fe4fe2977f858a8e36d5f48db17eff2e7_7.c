std::string ExtensionTtsController::GetMatchingExtensionId(
    Utterance* utterance) {
  ExtensionService* service = utterance->profile()->GetExtensionService();
  DCHECK(service);
  ExtensionEventRouter* event_router =
      utterance->profile()->GetExtensionEventRouter();
  DCHECK(event_router);
 
  const ExtensionList* extensions = service->extensions();
  ExtensionList::const_iterator iter;
  for (iter = extensions->begin(); iter != extensions->end(); ++iter) {
    const Extension* extension = *iter;
    if (!event_router->ExtensionHasEventListener(
            extension->id(), events::kOnSpeak) ||
        !event_router->ExtensionHasEventListener(
            extension->id(), events::kOnStop)) {
      continue;
     }
    const std::vector<Extension::TtsVoice>& tts_voices =
        extension->tts_voices();
    for (size_t i = 0; i < tts_voices.size(); ++i) {
      const Extension::TtsVoice& voice = tts_voices[i];
      if (!voice.voice_name.empty() &&
          !utterance->voice_name().empty() &&
          voice.voice_name != utterance->voice_name()) {
        continue;
      }
      if (!voice.locale.empty() &&
          !utterance->locale().empty() &&
          voice.locale != utterance->locale()) {
        continue;
      }
      if (!voice.gender.empty() &&
          !utterance->gender().empty() &&
          voice.gender != utterance->gender()) {
        continue;
      }
      return extension->id();
    }
  }
  return std::string();
}
