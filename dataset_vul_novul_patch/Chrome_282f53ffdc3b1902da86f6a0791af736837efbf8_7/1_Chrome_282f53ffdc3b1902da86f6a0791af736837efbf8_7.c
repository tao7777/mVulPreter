void DiceTurnSyncOnHelper::AbortAndDelete() {
   if (signin_aborted_mode_ == SigninAbortedMode::REMOVE_ACCOUNT) {
    token_service_->RevokeCredentials(account_info_.account_id);
   }
   delete this;
 }
