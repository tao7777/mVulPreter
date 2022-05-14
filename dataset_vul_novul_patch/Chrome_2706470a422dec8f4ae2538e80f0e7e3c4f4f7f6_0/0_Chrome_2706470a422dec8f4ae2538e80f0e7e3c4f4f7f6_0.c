void PaymentRequest::AreRequestedMethodsSupportedCallback(
void PaymentRequest::UpdateWith(mojom::PaymentDetailsPtr details) {
  if (!IsInitialized()) {
    log_.Error("Attempted updateWith without initialization");
     OnConnectionTerminated();
    return;
   }
