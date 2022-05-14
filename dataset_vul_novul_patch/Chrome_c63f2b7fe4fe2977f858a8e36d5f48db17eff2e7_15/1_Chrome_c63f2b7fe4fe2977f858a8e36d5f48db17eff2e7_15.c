void ExtensionTtsController::SpeakNow(Utterance* utterance) {
  std::string extension_id = GetMatchingExtensionId(utterance);
  if (!extension_id.empty()) {
    current_utterance_ = utterance;
    utterance->set_extension_id(extension_id);
    ListValue args;
    args.Set(0, Value::CreateStringValue(utterance->text()));
    DictionaryValue* options = static_cast<DictionaryValue*>(
        utterance->options()->DeepCopy());
    if (options->HasKey(util::kEnqueueKey))
      options->Remove(util::kEnqueueKey, NULL);
    args.Set(1, options);
    args.Set(2, Value::CreateIntegerValue(utterance->id()));
    std::string json_args;
    base::JSONWriter::Write(&args, false, &json_args);
    utterance->profile()->GetExtensionEventRouter()->DispatchEventToExtension(
        extension_id,
        events::kOnSpeak,
        json_args,
        utterance->profile(),
        GURL());
    return;
   }
 
  GetPlatformImpl()->clear_error();
  bool success = GetPlatformImpl()->Speak(
      utterance->text(),
      utterance->locale(),
      utterance->gender(),
      utterance->rate(),
      utterance->pitch(),
      utterance->volume());
  if (!success) {
    utterance->set_error(GetPlatformImpl()->error());
    utterance->FinishAndDestroy();
    return;
   }
  current_utterance_ = utterance;
  CheckSpeechStatus();
}
