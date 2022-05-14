Utterance::Utterance(Profile* profile,
                     const std::string& text,
                     DictionaryValue* options,
                     Task* completion_task)
    : profile_(profile),
      id_(next_utterance_id_++),
      text_(text),
      rate_(-1.0),
      pitch_(-1.0),
      volume_(-1.0),
      can_enqueue_(false),
      completion_task_(completion_task) {
  if (!options) {
    options_.reset(new DictionaryValue());
    return;
   }
 
  options_.reset(options->DeepCopy());
  if (options->HasKey(util::kVoiceNameKey))
    options->GetString(util::kVoiceNameKey, &voice_name_);
  if (options->HasKey(util::kLocaleKey))
    options->GetString(util::kLocaleKey, &locale_);
  if (options->HasKey(util::kGenderKey))
    options->GetString(util::kGenderKey, &gender_);
  if (options->GetDouble(util::kRateKey, &rate_)) {
    if (!base::IsFinite(rate_) || rate_ < 0.0 || rate_ > 1.0)
      rate_ = -1.0;
   }
 
  if (options->GetDouble(util::kPitchKey, &pitch_)) {
    if (!base::IsFinite(pitch_) || pitch_ < 0.0 || pitch_ > 1.0)
      pitch_ = -1.0;
   }
 
  if (options->GetDouble(util::kVolumeKey, &volume_)) {
    if (!base::IsFinite(volume_) || volume_ < 0.0 || volume_ > 1.0)
      volume_ = -1.0;
   }
 
  if (options->HasKey(util::kEnqueueKey))
    options->GetBoolean(util::kEnqueueKey, &can_enqueue_);
}
