void PageInfo::PresentSiteIdentity() {
  DCHECK_NE(site_identity_status_, SITE_IDENTITY_STATUS_UNKNOWN);
  DCHECK_NE(site_connection_status_, SITE_CONNECTION_STATUS_UNKNOWN);
  PageInfoUI::IdentityInfo info;
  if (site_identity_status_ == SITE_IDENTITY_STATUS_EV_CERT)
    info.site_identity = UTF16ToUTF8(organization_name());
  else
    info.site_identity = UTF16ToUTF8(GetSimpleSiteName(site_url_));

   info.connection_status = site_connection_status_;
   info.connection_status_description = UTF16ToUTF8(site_connection_details_);
   info.identity_status = site_identity_status_;
  info.safe_browsing_status = safe_browsing_status_;
  info.identity_status_description = UTF16ToUTF8(site_details_message_);
   info.certificate = certificate_;
   info.show_ssl_decision_revoke_button = show_ssl_decision_revoke_button_;
   info.show_change_password_buttons = show_change_password_buttons_;
  ui_->SetIdentityInfo(info);
}
