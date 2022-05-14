void PaymentRequest::UpdateWith(mojom::PaymentDetailsPtr details) {
   std::string error;
   if (!ValidatePaymentDetails(ConvertPaymentDetails(details), &error)) {
    log_.Error(error);
     OnConnectionTerminated();
     return;
   }
 
   if (details->shipping_address_errors &&
       !PaymentsValidators::IsValidAddressErrorsFormat(
           details->shipping_address_errors, &error)) {
    log_.Error(error);
     OnConnectionTerminated();
     return;
   }
 
   if (!details->total) {
    log_.Error("Missing total");
     OnConnectionTerminated();
     return;
   }

  spec_->UpdateWith(std::move(details));
 }
