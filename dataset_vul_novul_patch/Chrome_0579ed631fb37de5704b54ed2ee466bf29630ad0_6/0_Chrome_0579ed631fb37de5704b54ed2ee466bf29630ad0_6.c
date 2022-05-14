void NetworkThrottleManagerImpl::SetTickClockForTesting(
     std::unique_ptr<base::TickClock> tick_clock) {
   tick_clock_ = std::move(tick_clock);
   DCHECK(!outstanding_recomputation_timer_->IsRunning());
  outstanding_recomputation_timer_ = std::make_unique<base::Timer>(
       false /* retain_user_task */, false /* is_repeating */,
       tick_clock_.get());
 }
