void PageInfo::RecordPasswordReuseEvent() {
  if (!password_protection_service_) {
     return;
   }
 
  if (safe_browsing_status_ == SAFE_BROWSING_STATUS_SIGN_IN_PASSWORD_REUSE) {
     safe_browsing::LogWarningAction(
         safe_browsing::WarningUIType::PAGE_INFO,
         safe_browsing::WarningAction::SHOWN,
        safe_browsing::LoginReputationClientRequest::PasswordReuseEvent::
            SIGN_IN_PASSWORD,
        password_protection_service_->GetSyncAccountType());
  } else {
    safe_browsing::LogWarningAction(
        safe_browsing::WarningUIType::PAGE_INFO,
        safe_browsing::WarningAction::SHOWN,
        safe_browsing::LoginReputationClientRequest::PasswordReuseEvent::
            ENTERPRISE_PASSWORD,
        password_protection_service_->GetSyncAccountType());
  }
}
