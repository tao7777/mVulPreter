void ExtensionTtsController::SpeakNow(Utterance* utterance) {
   }
 
  double pitch = 1.0;
  if (options->HasKey(constants::kPitchKey)) {
    EXTENSION_FUNCTION_VALIDATE(
        options->GetDouble(constants::kPitchKey, &pitch));
    if (pitch < 0.0 || pitch > 2.0) {
      error_ = constants::kErrorInvalidPitch;
      return false;
    }
   }
