 void ChromeInvalidationClient::InformError(
     invalidation::InvalidationClient* client,
     const invalidation::ErrorInfo& error_info) {
   LOG(ERROR) << "Ticl error " << error_info.error_reason() << ": "
              << error_info.error_message()
              << " (transient = " << error_info.is_transient() << ")";
  if (error_info.error_reason() == invalidation::ErrorReason::AUTH_FAILURE) {
    ticl_state_ = NOTIFICATION_CREDENTIALS_REJECTED;
  } else {
    ticl_state_ = TRANSIENT_NOTIFICATION_ERROR;
  }
  EmitStateChange();
}
