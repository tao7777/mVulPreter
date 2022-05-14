void CloudPolicySubsystem::StopAutoRetry() {
  cloud_policy_controller_->StopAutoRetry();
  device_token_fetcher_->StopAutoRetry();
   data_store_->Reset();
 }
