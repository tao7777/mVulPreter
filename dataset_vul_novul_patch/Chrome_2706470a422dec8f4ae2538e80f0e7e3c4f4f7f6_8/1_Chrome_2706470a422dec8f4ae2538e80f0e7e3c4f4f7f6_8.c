 void PaymentRequest::Show(bool is_user_gesture) {
  if (!client_.is_bound() || !binding_.is_bound()) {
    LOG(ERROR) << "Attempted Show(), but binding(s) missing.";
     OnConnectionTerminated();
     return;
   }
 
   display_handle_ = display_manager_->TryShow(delegate_.get());
   if (!display_handle_) {
    LOG(ERROR) << "A PaymentRequest UI is already showing";
     journey_logger_.SetNotShown(
         JourneyLogger::NOT_SHOWN_REASON_CONCURRENT_REQUESTS);
     client_->OnError(mojom::PaymentErrorReason::ALREADY_SHOWING);
    OnConnectionTerminated();
    return;
   }
 
   if (!delegate_->IsBrowserWindowActive()) {
    LOG(ERROR) << "Cannot show PaymentRequest UI in a background tab";
     journey_logger_.SetNotShown(JourneyLogger::NOT_SHOWN_REASON_OTHER);
     client_->OnError(mojom::PaymentErrorReason::USER_CANCEL);
     OnConnectionTerminated();
     return;
   }
 
   if (!state_) {
     AreRequestedMethodsSupportedCallback(false);
     return;
   }

  is_show_user_gesture_ = is_user_gesture;

  display_handle_->Show(this);

  state_->AreRequestedMethodsSupported(
      base::BindOnce(&PaymentRequest::AreRequestedMethodsSupportedCallback,
                     weak_ptr_factory_.GetWeakPtr()));
 }
