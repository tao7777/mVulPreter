void ExtensionTtsController::SpeakOrEnqueue(Utterance* utterance) {
  std::string gender;
  if (options->HasKey(constants::kGenderKey))
    EXTENSION_FUNCTION_VALIDATE(
        options->GetString(constants::kGenderKey, &gender));
  if (!gender.empty() &&
      gender != constants::kGenderFemale &&
      gender != constants::kGenderMale) {
    error_ = constants::kErrorInvalidGender;
    return false;
   }
