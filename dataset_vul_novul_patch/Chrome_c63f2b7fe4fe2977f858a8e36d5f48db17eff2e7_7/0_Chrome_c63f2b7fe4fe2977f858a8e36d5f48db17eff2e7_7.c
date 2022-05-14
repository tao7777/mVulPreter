std::string ExtensionTtsController::GetMatchingExtensionId(
 
  double rate = 1.0;
  if (options->HasKey(constants::kRateKey)) {
    EXTENSION_FUNCTION_VALIDATE(
        options->GetDouble(constants::kRateKey, &rate));
    if (rate < 0.1 || rate > 10.0) {
      error_ = constants::kErrorInvalidRate;
      return false;
     }
