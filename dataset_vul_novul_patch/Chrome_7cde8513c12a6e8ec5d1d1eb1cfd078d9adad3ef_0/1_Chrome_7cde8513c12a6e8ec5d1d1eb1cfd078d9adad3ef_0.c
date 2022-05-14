void PageInfo::GetSafeBrowsingStatusByMaliciousContentStatus(
     security_state::MaliciousContentStatus malicious_content_status,
    PageInfo::SafeBrowsingStatus* status,
     base::string16* details) {
   switch (malicious_content_status) {
     case security_state::MALICIOUS_CONTENT_STATUS_NONE:
       NOTREACHED();
       break;
     case security_state::MALICIOUS_CONTENT_STATUS_MALWARE:
      *status = PageInfo::SAFE_BROWSING_STATUS_MALWARE;
       *details = l10n_util::GetStringUTF16(IDS_PAGE_INFO_MALWARE_DETAILS);
       break;
     case security_state::MALICIOUS_CONTENT_STATUS_SOCIAL_ENGINEERING:
      *status = PageInfo::SAFE_BROWSING_STATUS_SOCIAL_ENGINEERING;
       *details =
           l10n_util::GetStringUTF16(IDS_PAGE_INFO_SOCIAL_ENGINEERING_DETAILS);
       break;
     case security_state::MALICIOUS_CONTENT_STATUS_UNWANTED_SOFTWARE:
      *status = PageInfo::SAFE_BROWSING_STATUS_UNWANTED_SOFTWARE;
       *details =
           l10n_util::GetStringUTF16(IDS_PAGE_INFO_UNWANTED_SOFTWARE_DETAILS);
       break;
     case security_state::MALICIOUS_CONTENT_STATUS_SIGN_IN_PASSWORD_REUSE:
 #if defined(FULL_SAFE_BROWSING)
      *status = PageInfo::SAFE_BROWSING_STATUS_SIGN_IN_PASSWORD_REUSE;
       *details = password_protection_service_
                      ? password_protection_service_->GetWarningDetailText(
                           PasswordReuseEvent::SIGN_IN_PASSWORD)
                     : base::string16();
#endif
       break;
     case security_state::MALICIOUS_CONTENT_STATUS_ENTERPRISE_PASSWORD_REUSE:
 #if defined(FULL_SAFE_BROWSING)
      *status = PageInfo::SAFE_BROWSING_STATUS_ENTERPRISE_PASSWORD_REUSE;
       *details = password_protection_service_
                      ? password_protection_service_->GetWarningDetailText(
                           PasswordReuseEvent::ENTERPRISE_PASSWORD)
                     : base::string16();
 #endif
       break;
     case security_state::MALICIOUS_CONTENT_STATUS_BILLING:
      *status = PageInfo::SAFE_BROWSING_STATUS_BILLING;
       *details = l10n_util::GetStringUTF16(IDS_PAGE_INFO_BILLING_DETAILS);
       break;
   }
}
