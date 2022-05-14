 void PaymentRequest::Show(bool is_user_gesture) {
  if (!IsInitialized()) {
    log_.Error("Attempted show without initialization");
    OnConnectionTerminated();
    return;
  }

  if (is_show_called_) {
    log_.Error("Attempted show twice");
     OnConnectionTerminated();
     return;
   }
 
  is_show_called_ = true;

   display_handle_ = display_manager_->TryShow(delegate_.get());
   if (!display_handle_) {
    log_.Error("A PaymentRequest UI is already showing");
     journey_logger_.SetNotShown(
         JourneyLogger::NOT_SHOWN_REASON_CONCURRENT_REQUESTS);
     client_->OnError(mojom::PaymentErrorReason::ALREADY_SHOWING);
    OnConnectionTerminated();
    return;
   }
 
   if (!delegate_->IsBrowserWindowActive()) {
    log_.Error("Cannot show PaymentRequest UI in a background tab");
     journey_logger_.SetNotShown(JourneyLogger::NOT_SHOWN_REASON_OTHER);
     client_->OnError(mojom::PaymentErrorReason::USER_CANCEL);
     OnConnectionTerminated();
     return;
   }
 
   if (!state_) {
    // SSL is not valid. Reject show with NotSupportedError, disconnect the
    // mojo pipe, and destroy this object.
     AreRequestedMethodsSupportedCallback(false);
     return;
   }

  is_show_user_gesture_ = is_user_gesture;

  display_handle_->Show(this);

  state_->AreRequestedMethodsSupported(
      base::BindOnce(&PaymentRequest::AreRequestedMethodsSupportedCallback,
                     weak_ptr_factory_.GetWeakPtr()));
 }
