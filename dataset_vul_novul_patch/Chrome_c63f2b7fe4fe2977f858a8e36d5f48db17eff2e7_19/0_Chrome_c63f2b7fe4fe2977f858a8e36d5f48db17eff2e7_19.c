Utterance::Utterance(Profile* profile,
bool ExtensionTtsSpeakFunction::RunImpl() {
  std::string text;
  EXTENSION_FUNCTION_VALIDATE(args_->GetString(0, &text));
  if (text.size() > 32768) {
    error_ = constants::kErrorUtteranceTooLong;
    return false;
   }
 
  scoped_ptr<DictionaryValue> options;
  if (args_->GetSize() >= 2) {
    DictionaryValue* temp_options = NULL;
    EXTENSION_FUNCTION_VALIDATE(args_->GetDictionary(1, &temp_options));
    options.reset(temp_options->DeepCopy());
   }
 
  std::string voice_name;
  if (options->HasKey(constants::kVoiceNameKey)) {
    EXTENSION_FUNCTION_VALIDATE(
        options->GetString(constants::kVoiceNameKey, &voice_name));
   }
 
  std::string lang;
  if (options->HasKey(constants::kLangKey))
    EXTENSION_FUNCTION_VALIDATE(options->GetString(constants::kLangKey, &lang));
  if (!lang.empty() && !l10n_util::IsValidLocaleSyntax(lang)) {
    error_ = constants::kErrorInvalidLang;
    return false;
   }
 
