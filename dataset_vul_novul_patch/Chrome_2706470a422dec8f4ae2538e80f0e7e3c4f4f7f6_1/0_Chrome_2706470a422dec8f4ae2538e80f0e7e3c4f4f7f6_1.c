 void PaymentRequest::CanMakePayment() {
  if (!IsInitialized()) {
    log_.Error("Attempted canMakePayment without initialization");
    OnConnectionTerminated();
    return;
  }

  // It's valid to call canMakePayment() without calling show() first.

   if (observer_for_testing_)
     observer_for_testing_->OnCanMakePaymentCalled();
 
  if (!delegate_->GetPrefService()->GetBoolean(kCanMakePaymentEnabled) ||
      !state_) {
    CanMakePaymentCallback(/*can_make_payment=*/false);
  } else {
    state_->CanMakePayment(
        base::BindOnce(&PaymentRequest::CanMakePaymentCallback,
                       weak_ptr_factory_.GetWeakPtr()));
   }
 }
