 void PaymentRequest::Retry(mojom::PaymentValidationErrorsPtr errors) {
  if (!IsInitialized()) {
    log_.Error("Attempted retry without initialization");
     OnConnectionTerminated();
     return;
   }
 
  if (!IsThisPaymentRequestShowing()) {
    log_.Error("Attempted retry without show");
     OnConnectionTerminated();
     return;
   }
 
   std::string error;
   if (!PaymentsValidators::IsValidPaymentValidationErrorsFormat(errors,
                                                                 &error)) {
    log_.Error(error);
     client_->OnError(mojom::PaymentErrorReason::USER_CANCEL);
     OnConnectionTerminated();
     return;
  }

  spec()->Retry(std::move(errors));
   display_handle_->Retry();
 }
