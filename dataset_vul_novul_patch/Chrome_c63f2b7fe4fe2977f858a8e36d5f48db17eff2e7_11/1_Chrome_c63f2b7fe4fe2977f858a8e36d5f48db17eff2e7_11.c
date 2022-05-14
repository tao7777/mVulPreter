bool ExtensionTtsSpeakFunction::RunImpl() {
  std::string text;
  EXTENSION_FUNCTION_VALIDATE(args_->GetString(0, &text));
  DictionaryValue* options = NULL;
  if (args_->GetSize() >= 2)
    EXTENSION_FUNCTION_VALIDATE(args_->GetDictionary(1, &options));
  Task* completion_task = NewRunnableMethod(
      this, &ExtensionTtsSpeakFunction::SpeechFinished);
  utterance_ = new Utterance(profile(), text, options, completion_task);
  AddRef();  // Balanced in SpeechFinished().
  ExtensionTtsController::GetInstance()->SpeakOrEnqueue(utterance_);
   return true;
 }
