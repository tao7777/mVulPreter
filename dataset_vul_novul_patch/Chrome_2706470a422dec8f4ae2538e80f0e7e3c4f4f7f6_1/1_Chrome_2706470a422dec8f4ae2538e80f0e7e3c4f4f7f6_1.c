 void PaymentRequest::CanMakePayment() {
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
