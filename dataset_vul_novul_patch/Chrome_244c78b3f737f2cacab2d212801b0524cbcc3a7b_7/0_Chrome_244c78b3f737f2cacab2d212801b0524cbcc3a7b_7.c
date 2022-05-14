void DeviceTokenFetcher::SetState(FetcherState state) {
  state_ = state;
   if (state_ != STATE_TEMPORARY_ERROR)
     effective_token_fetch_error_delay_ms_ = kTokenFetchErrorDelayMilliseconds;
 
  backend_.reset();  // Stop any pending requests.

   base::Time delayed_work_at;
   switch (state_) {
     case STATE_INACTIVE:
      notifier_->Inform(CloudPolicySubsystem::UNENROLLED,
                        CloudPolicySubsystem::NO_DETAILS,
                        PolicyNotifier::TOKEN_FETCHER);
      break;
    case STATE_TOKEN_AVAILABLE:
      notifier_->Inform(CloudPolicySubsystem::SUCCESS,
                        CloudPolicySubsystem::NO_DETAILS,
                        PolicyNotifier::TOKEN_FETCHER);
      break;
    case STATE_UNMANAGED:
      delayed_work_at = cache_->last_policy_refresh_time() +
          base::TimeDelta::FromMilliseconds(
              kUnmanagedDeviceRefreshRateMilliseconds);
      notifier_->Inform(CloudPolicySubsystem::UNMANAGED,
                        CloudPolicySubsystem::NO_DETAILS,
                        PolicyNotifier::TOKEN_FETCHER);
      break;
    case STATE_TEMPORARY_ERROR:
      delayed_work_at = base::Time::Now() +
          base::TimeDelta::FromMilliseconds(
              effective_token_fetch_error_delay_ms_);
      effective_token_fetch_error_delay_ms_ =
          std::min(effective_token_fetch_error_delay_ms_ * 2,
                   kTokenFetchErrorMaxDelayMilliseconds);
      notifier_->Inform(CloudPolicySubsystem::NETWORK_ERROR,
                        CloudPolicySubsystem::DMTOKEN_NETWORK_ERROR,
                        PolicyNotifier::TOKEN_FETCHER);
      break;
    case STATE_ERROR:
      effective_token_fetch_error_delay_ms_ =
          kTokenFetchErrorMaxDelayMilliseconds;
      delayed_work_at = base::Time::Now() +
          base::TimeDelta::FromMilliseconds(
              effective_token_fetch_error_delay_ms_);
      notifier_->Inform(CloudPolicySubsystem::NETWORK_ERROR,
                        CloudPolicySubsystem::DMTOKEN_NETWORK_ERROR,
                        PolicyNotifier::TOKEN_FETCHER);
      break;
    case STATE_BAD_AUTH:
      notifier_->Inform(CloudPolicySubsystem::BAD_GAIA_TOKEN,
                        CloudPolicySubsystem::NO_DETAILS,
                        PolicyNotifier::TOKEN_FETCHER);
      break;
  }

  scheduler_->CancelDelayedWork();
  if (!delayed_work_at.is_null()) {
    base::Time now(base::Time::Now());
    int64 delay = std::max<int64>((delayed_work_at - now).InMilliseconds(), 0);
    scheduler_->PostDelayedWork(
        base::Bind(&DeviceTokenFetcher::DoWork, base::Unretained(this)), delay);
  }
}
