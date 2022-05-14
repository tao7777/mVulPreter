void PageInfo::RecordPasswordReuseEvent() {
  if (!password_protection_service_) {
     return;
   }
 
  if (site_identity_status_ == SITE_IDENTITY_STATUS_SIGN_IN_PASSWORD_REUSE) {
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
