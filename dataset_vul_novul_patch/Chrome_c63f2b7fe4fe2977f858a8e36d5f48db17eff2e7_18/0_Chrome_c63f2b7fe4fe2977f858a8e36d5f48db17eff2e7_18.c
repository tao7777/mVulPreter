void ExtensionTtsController::Stop() {
  double volume = 1.0;
  if (options->HasKey(constants::kVolumeKey)) {
    EXTENSION_FUNCTION_VALIDATE(
        options->GetDouble(constants::kVolumeKey, &volume));
    if (volume < 0.0 || volume > 1.0) {
      error_ = constants::kErrorInvalidVolume;
      return false;
    }
   }
 
