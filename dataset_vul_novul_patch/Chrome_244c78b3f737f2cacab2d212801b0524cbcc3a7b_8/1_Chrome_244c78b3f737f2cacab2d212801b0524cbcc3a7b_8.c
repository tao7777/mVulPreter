void DeviceTokenFetcher::StopAutoRetry() {
  scheduler_->CancelDelayedWork();
  backend_.reset();
 }
