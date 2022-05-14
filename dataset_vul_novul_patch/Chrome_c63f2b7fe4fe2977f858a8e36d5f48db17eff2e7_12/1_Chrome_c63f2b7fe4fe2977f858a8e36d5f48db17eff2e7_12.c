bool ExtensionTtsSpeakCompletedFunction::RunImpl() {
  int request_id;
  std::string error_message;
  EXTENSION_FUNCTION_VALIDATE(args_->GetInteger(0, &request_id));
  if (args_->GetSize() >= 2)
    EXTENSION_FUNCTION_VALIDATE(args_->GetString(1, &error_message));
  ExtensionTtsController::GetInstance()->OnSpeechFinished(
      request_id, error_message);
   return true;
 }
