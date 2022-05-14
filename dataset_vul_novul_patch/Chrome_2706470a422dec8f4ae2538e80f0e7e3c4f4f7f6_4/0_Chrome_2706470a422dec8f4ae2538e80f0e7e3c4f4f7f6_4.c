 void PaymentRequest::NoUpdatedPaymentDetails() {
  // This Mojo call is triggered by the user of the API doing nothing in
  // response to a shipping address update event, so the error messages cannot
  // be more verbose.
  if (!IsInitialized()) {
    log_.Error("Not initialized");
    OnConnectionTerminated();
    return;
  }

  if (!IsThisPaymentRequestShowing()) {
    log_.Error("Not shown");
    OnConnectionTerminated();
    return;
  }

   spec_->RecomputeSpecForDetails();
 }
