void ExtensionTtsController::FinishCurrentUtterance() {
  bool can_enqueue = false;
  if (options->HasKey(constants::kEnqueueKey)) {
    EXTENSION_FUNCTION_VALIDATE(
        options->GetBoolean(constants::kEnqueueKey, &can_enqueue));
   }
