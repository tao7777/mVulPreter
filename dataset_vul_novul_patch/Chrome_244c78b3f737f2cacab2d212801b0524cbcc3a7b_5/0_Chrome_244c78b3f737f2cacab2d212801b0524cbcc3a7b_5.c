void CloudPolicyController::StopAutoRetry() {
void CloudPolicyController::Reset() {
  SetState(STATE_TOKEN_UNAVAILABLE);
 }
