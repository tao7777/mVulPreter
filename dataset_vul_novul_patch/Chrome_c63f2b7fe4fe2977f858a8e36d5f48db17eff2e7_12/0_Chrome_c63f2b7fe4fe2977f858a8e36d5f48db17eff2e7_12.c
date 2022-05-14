bool ExtensionTtsSpeakCompletedFunction::RunImpl() {
bool ExtensionTtsGetVoicesFunction::RunImpl() {
  result_.reset(ExtensionTtsController::GetInstance()->GetVoices(profile()));
   return true;
 }
