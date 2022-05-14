void CloudPolicyController::StopAutoRetry() {
  scheduler_->CancelDelayedWork();
  backend_.reset();
 }
