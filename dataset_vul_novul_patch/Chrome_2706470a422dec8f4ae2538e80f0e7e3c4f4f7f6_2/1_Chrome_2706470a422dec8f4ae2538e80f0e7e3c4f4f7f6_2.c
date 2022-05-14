 void PaymentRequest::Complete(mojom::PaymentComplete result) {
  if (!client_.is_bound())
     return;
 
  if (result == mojom::PaymentComplete::FAIL) {
    delegate_->ShowErrorMessage();
  } else {
    DCHECK(!has_recorded_completion_);
    journey_logger_.SetCompleted();
    has_recorded_completion_ = true;

    delegate_->GetPrefService()->SetBoolean(kPaymentsFirstTransactionCompleted,
                                            true);
    client_->OnComplete();
    state_->RecordUseStats();
  }
 }
