 void CloudPolicyController::SetState(
     CloudPolicyController::ControllerState new_state) {
   state_ = new_state;

  backend_.reset();  // Stop any pending requests.
 
   base::Time now(base::Time::NowFromSystemTime());
   base::Time refresh_at;
  base::Time last_refresh(cache_->last_policy_refresh_time());
  if (last_refresh.is_null())
    last_refresh = now;

  bool inform_notifier_done = false;
  switch (state_) {
    case STATE_TOKEN_UNMANAGED:
      notifier_->Inform(CloudPolicySubsystem::UNMANAGED,
                        CloudPolicySubsystem::NO_DETAILS,
                        PolicyNotifier::POLICY_CONTROLLER);
      break;
    case STATE_TOKEN_UNAVAILABLE:
    case STATE_TOKEN_VALID:
      refresh_at = now;
      break;
    case STATE_POLICY_VALID:
      effective_policy_refresh_error_delay_ms_ =
          kPolicyRefreshErrorDelayInMilliseconds;
      refresh_at =
          last_refresh + base::TimeDelta::FromMilliseconds(GetRefreshDelay());
      notifier_->Inform(CloudPolicySubsystem::SUCCESS,
                        CloudPolicySubsystem::NO_DETAILS,
                        PolicyNotifier::POLICY_CONTROLLER);
      break;
    case STATE_TOKEN_ERROR:
      notifier_->Inform(CloudPolicySubsystem::NETWORK_ERROR,
                        CloudPolicySubsystem::BAD_DMTOKEN,
                        PolicyNotifier::POLICY_CONTROLLER);
      inform_notifier_done = true;
    case STATE_POLICY_ERROR:
      if (!inform_notifier_done) {
        notifier_->Inform(CloudPolicySubsystem::NETWORK_ERROR,
                          CloudPolicySubsystem::POLICY_NETWORK_ERROR,
                          PolicyNotifier::POLICY_CONTROLLER);
      }
      refresh_at = now + base::TimeDelta::FromMilliseconds(
                             effective_policy_refresh_error_delay_ms_);
      effective_policy_refresh_error_delay_ms_ =
          std::min(effective_policy_refresh_error_delay_ms_ * 2,
                   policy_refresh_rate_ms_);
      break;
    case STATE_POLICY_UNAVAILABLE:
      effective_policy_refresh_error_delay_ms_ = policy_refresh_rate_ms_;
      refresh_at = now + base::TimeDelta::FromMilliseconds(
                             effective_policy_refresh_error_delay_ms_);
      notifier_->Inform(CloudPolicySubsystem::NETWORK_ERROR,
                        CloudPolicySubsystem::POLICY_NETWORK_ERROR,
                        PolicyNotifier::POLICY_CONTROLLER);
      break;
  }

  scheduler_->CancelDelayedWork();
  if (!refresh_at.is_null()) {
    int64 delay = std::max<int64>((refresh_at - now).InMilliseconds(), 0);
    scheduler_->PostDelayedWork(
        base::Bind(&CloudPolicyController::DoWork, base::Unretained(this)),
        delay);
  }
}
