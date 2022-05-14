void PaymentRequest::AreRequestedMethodsSupportedCallback(
    bool methods_supported) {
  if (methods_supported) {
    if (SatisfiesSkipUIConstraints()) {
      skipped_payment_request_ui_ = true;
      Pay();
    }
  } else {
    journey_logger_.SetNotShown(
        JourneyLogger::NOT_SHOWN_REASON_NO_SUPPORTED_PAYMENT_METHOD);
    client_->OnError(mojom::PaymentErrorReason::NOT_SUPPORTED);
    if (observer_for_testing_)
      observer_for_testing_->OnNotSupportedError();
     OnConnectionTerminated();
   }
}
