 void PaymentRequest::Complete(mojom::PaymentComplete result) {
  if (!IsInitialized()) {
    log_.Error("Attempted complete without initialization");
    OnConnectionTerminated();
     return;
  }

  if (!IsThisPaymentRequestShowing()) {
    log_.Error("Attempted complete without show");
    OnConnectionTerminated();
    return;
  }
 
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
