 void PaymentRequest::Abort() {

  bool accepting_abort = !state_->IsPaymentAppInvoked();
  if (accepting_abort)
    RecordFirstAbortReason(JourneyLogger::ABORT_REASON_ABORTED_BY_MERCHANT);

  if (client_.is_bound())
    client_->OnAbort(accepting_abort);

  if (observer_for_testing_)
    observer_for_testing_->OnAbortCalled();
 }
