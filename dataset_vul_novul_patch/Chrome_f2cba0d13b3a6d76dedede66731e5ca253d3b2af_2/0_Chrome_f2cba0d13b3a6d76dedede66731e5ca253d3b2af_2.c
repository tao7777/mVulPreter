 void WebsiteSettings::OnUIClosing() {
  if (show_info_bar_ && web_contents_) {
    InfoBarService* infobar_service =
        InfoBarService::FromWebContents(web_contents_);
    if (infobar_service)
      WebsiteSettingsInfoBarDelegate::Create(infobar_service);
  }
 
   SSLCertificateDecisionsDidRevoke user_decision =
       did_revoke_user_ssl_decisions_ ? USER_CERT_DECISIONS_REVOKED
                                     : USER_CERT_DECISIONS_NOT_REVOKED;

  UMA_HISTOGRAM_ENUMERATION("interstitial.ssl.did_user_revoke_decisions",
                            user_decision,
                            END_OF_SSL_CERTIFICATE_DECISIONS_DID_REVOKE_ENUM);
}
