void PageInfo::ComputeUIInputs(
    const GURL& url,
    security_state::SecurityLevel security_level,
    const security_state::VisibleSecurityState& visible_security_state) {
#if !defined(OS_ANDROID)
  DCHECK(!url.SchemeIs(content::kChromeUIScheme) &&
         !url.SchemeIs(content::kChromeDevToolsScheme) &&
         !url.SchemeIs(content::kViewSourceScheme) &&
         !url.SchemeIs(content_settings::kExtensionScheme));
#endif

  bool is_chrome_ui_native_scheme = false;
#if defined(OS_ANDROID)
  is_chrome_ui_native_scheme = url.SchemeIs(chrome::kChromeUINativeScheme);
#endif

  security_level_ = security_level;

  if (url.SchemeIs(url::kAboutScheme)) {
     DCHECK_EQ(url::kAboutBlankURL, url.spec());
     site_identity_status_ = SITE_IDENTITY_STATUS_NO_CERT;
    site_details_message_ =
         l10n_util::GetStringUTF16(IDS_PAGE_INFO_SECURITY_TAB_INSECURE_IDENTITY);
     site_connection_status_ = SITE_CONNECTION_STATUS_UNENCRYPTED;
     site_connection_details_ = l10n_util::GetStringFUTF16(
        IDS_PAGE_INFO_SECURITY_TAB_NOT_ENCRYPTED_CONNECTION_TEXT,
        UTF8ToUTF16(url.spec()));
    return;
  }
 
   if (url.SchemeIs(content::kChromeUIScheme) || is_chrome_ui_native_scheme) {
     site_identity_status_ = SITE_IDENTITY_STATUS_INTERNAL_PAGE;
    site_details_message_ =
         l10n_util::GetStringUTF16(IDS_PAGE_INFO_INTERNAL_PAGE);
     site_connection_status_ = SITE_CONNECTION_STATUS_INTERNAL_PAGE;
     return;
  }

   certificate_ = visible_security_state.certificate;
 
  if (certificate_ &&
      (!net::IsCertStatusError(visible_security_state.cert_status) ||
       net::IsCertStatusMinorError(visible_security_state.cert_status))) {
     if (security_level == security_state::SECURE_WITH_POLICY_INSTALLED_CERT) {
 #if defined(OS_CHROMEOS)
       site_identity_status_ = SITE_IDENTITY_STATUS_ADMIN_PROVIDED_CERT;
      site_details_message_ = l10n_util::GetStringFUTF16(
           IDS_CERT_POLICY_PROVIDED_CERT_MESSAGE, UTF8ToUTF16(url.host()));
 #else
       DCHECK(false) << "Policy certificates exist only on ChromeOS";
#endif
    } else if (net::IsCertStatusMinorError(
                   visible_security_state.cert_status)) {
      site_identity_status_ = SITE_IDENTITY_STATUS_CERT_REVOCATION_UNKNOWN;
      base::string16 issuer_name(
          UTF8ToUTF16(certificate_->issuer().GetDisplayName()));
      if (issuer_name.empty()) {
        issuer_name.assign(l10n_util::GetStringUTF16(
             IDS_PAGE_INFO_SECURITY_TAB_UNKNOWN_PARTY));
       }
 
      site_details_message_.assign(l10n_util::GetStringFUTF16(
           IDS_PAGE_INFO_SECURITY_TAB_SECURE_IDENTITY_VERIFIED, issuer_name));
 
      site_details_message_ += ASCIIToUTF16("\n\n");
       if (visible_security_state.cert_status &
           net::CERT_STATUS_UNABLE_TO_CHECK_REVOCATION) {
        site_details_message_ += l10n_util::GetStringUTF16(
             IDS_PAGE_INFO_SECURITY_TAB_UNABLE_TO_CHECK_REVOCATION);
       } else if (visible_security_state.cert_status &
                  net::CERT_STATUS_NO_REVOCATION_MECHANISM) {
        site_details_message_ += l10n_util::GetStringUTF16(
             IDS_PAGE_INFO_SECURITY_TAB_NO_REVOCATION_MECHANISM);
       } else {
         NOTREACHED() << "Need to specify string for this warning";
      }
    } else {
      if (visible_security_state.cert_status & net::CERT_STATUS_IS_EV) {
        site_identity_status_ = SITE_IDENTITY_STATUS_EV_CERT;
        DCHECK(!certificate_->subject().organization_names.empty());
        organization_name_ =
            UTF8ToUTF16(certificate_->subject().organization_names[0]);
         DCHECK(!certificate_->subject().locality_name.empty());
         DCHECK(!certificate_->subject().country_name.empty());
        site_details_message_.assign(l10n_util::GetStringFUTF16(
             IDS_PAGE_INFO_SECURITY_TAB_SECURE_IDENTITY_EV_VERIFIED,
             organization_name_,
             UTF8ToUTF16(certificate_->subject().country_name)));
      } else {
        site_identity_status_ = SITE_IDENTITY_STATUS_CERT;
        base::string16 issuer_name(
            UTF8ToUTF16(certificate_->issuer().GetDisplayName()));
        if (issuer_name.empty()) {
          issuer_name.assign(l10n_util::GetStringUTF16(
               IDS_PAGE_INFO_SECURITY_TAB_UNKNOWN_PARTY));
         }
 
        site_details_message_.assign(l10n_util::GetStringFUTF16(
             IDS_PAGE_INFO_SECURITY_TAB_SECURE_IDENTITY_VERIFIED, issuer_name));
       }
       if (security_state::IsSHA1InChain(visible_security_state)) {
         site_identity_status_ =
             SITE_IDENTITY_STATUS_DEPRECATED_SIGNATURE_ALGORITHM;
        site_details_message_ +=
             UTF8ToUTF16("\n\n") +
             l10n_util::GetStringUTF16(
                 IDS_PAGE_INFO_SECURITY_TAB_DEPRECATED_SIGNATURE_ALGORITHM);
       }
     }
   } else {
    site_details_message_.assign(l10n_util::GetStringUTF16(
         IDS_PAGE_INFO_SECURITY_TAB_INSECURE_IDENTITY));
     if (!security_state::IsSchemeCryptographic(visible_security_state.url) ||
         !visible_security_state.certificate) {
      site_identity_status_ = SITE_IDENTITY_STATUS_NO_CERT;
    } else {
      site_identity_status_ = SITE_IDENTITY_STATUS_ERROR;
    }

    const base::string16 bullet = UTF8ToUTF16("\n • ");
    std::vector<ssl_errors::ErrorInfo> errors;
     ssl_errors::ErrorInfo::GetErrorsForCertStatus(
         certificate_, visible_security_state.cert_status, url, &errors);
     for (size_t i = 0; i < errors.size(); ++i) {
      site_details_message_ += bullet;
      site_details_message_ += errors[i].short_description();
     }
 
     if (visible_security_state.cert_status & net::CERT_STATUS_NON_UNIQUE_NAME) {
      site_details_message_ += ASCIIToUTF16("\n\n");
      site_details_message_ +=
           l10n_util::GetStringUTF16(IDS_PAGE_INFO_SECURITY_TAB_NON_UNIQUE_NAME);
     }
   }
 
  if (visible_security_state.malicious_content_status !=
      security_state::MALICIOUS_CONTENT_STATUS_NONE) {
    GetSafeBrowsingStatusByMaliciousContentStatus(
        visible_security_state.malicious_content_status, &safe_browsing_status_,
        &site_details_message_);
#if defined(FULL_SAFE_BROWSING)
    bool old_show_change_pw_buttons = show_change_password_buttons_;
#endif
    show_change_password_buttons_ =
        (visible_security_state.malicious_content_status ==
             security_state::MALICIOUS_CONTENT_STATUS_SIGN_IN_PASSWORD_REUSE ||
         visible_security_state.malicious_content_status ==
             security_state::
                 MALICIOUS_CONTENT_STATUS_ENTERPRISE_PASSWORD_REUSE);
#if defined(FULL_SAFE_BROWSING)
    if (show_change_password_buttons_ && !old_show_change_pw_buttons) {
      RecordPasswordReuseEvent();
    }
#endif
  }
  site_connection_status_ = SITE_CONNECTION_STATUS_UNKNOWN;

  base::string16 subject_name(GetSimpleSiteName(url));
  if (subject_name.empty()) {
    subject_name.assign(
        l10n_util::GetStringUTF16(IDS_PAGE_INFO_SECURITY_TAB_UNKNOWN_PARTY));
  }

  if (!visible_security_state.certificate ||
      !security_state::IsSchemeCryptographic(visible_security_state.url)) {
    site_connection_status_ = SITE_CONNECTION_STATUS_UNENCRYPTED;

    site_connection_details_.assign(l10n_util::GetStringFUTF16(
        IDS_PAGE_INFO_SECURITY_TAB_NOT_ENCRYPTED_CONNECTION_TEXT,
        subject_name));
  } else if (!visible_security_state.connection_info_initialized) {
    DCHECK_NE(security_level, security_state::NONE);
    site_connection_status_ = SITE_CONNECTION_STATUS_ENCRYPTED_ERROR;
  } else {
    site_connection_status_ = SITE_CONNECTION_STATUS_ENCRYPTED;

    if (net::ObsoleteSSLStatus(
            visible_security_state.connection_status,
            visible_security_state.peer_signature_algorithm) ==
        net::OBSOLETE_SSL_NONE) {
      site_connection_details_.assign(l10n_util::GetStringFUTF16(
          IDS_PAGE_INFO_SECURITY_TAB_ENCRYPTED_CONNECTION_TEXT, subject_name));
    } else {
      site_connection_details_.assign(l10n_util::GetStringFUTF16(
          IDS_PAGE_INFO_SECURITY_TAB_WEAK_ENCRYPTION_CONNECTION_TEXT,
          subject_name));
    }

    ReportAnyInsecureContent(visible_security_state, &site_connection_status_,
                             &site_connection_details_);
  }

  uint16_t cipher_suite = net::SSLConnectionStatusToCipherSuite(
      visible_security_state.connection_status);
  if (visible_security_state.connection_info_initialized && cipher_suite) {
    int ssl_version = net::SSLConnectionStatusToVersion(
        visible_security_state.connection_status);
    const char* ssl_version_str;
    net::SSLVersionToString(&ssl_version_str, ssl_version);
    site_connection_details_ += ASCIIToUTF16("\n\n");
    site_connection_details_ += l10n_util::GetStringFUTF16(
        IDS_PAGE_INFO_SECURITY_TAB_SSL_VERSION, ASCIIToUTF16(ssl_version_str));

    const char *key_exchange, *cipher, *mac;
    bool is_aead, is_tls13;
    net::SSLCipherSuiteToStrings(&key_exchange, &cipher, &mac, &is_aead,
                                 &is_tls13, cipher_suite);

    site_connection_details_ += ASCIIToUTF16("\n\n");
    if (is_aead) {
      if (is_tls13) {
        key_exchange =
            SSL_get_curve_name(visible_security_state.key_exchange_group);
        if (!key_exchange) {
          NOTREACHED();
          key_exchange = "";
        }
      }
      site_connection_details_ += l10n_util::GetStringFUTF16(
          IDS_PAGE_INFO_SECURITY_TAB_ENCRYPTION_DETAILS_AEAD,
          ASCIIToUTF16(cipher), ASCIIToUTF16(key_exchange));
    } else {
      site_connection_details_ += l10n_util::GetStringFUTF16(
          IDS_PAGE_INFO_SECURITY_TAB_ENCRYPTION_DETAILS, ASCIIToUTF16(cipher),
          ASCIIToUTF16(mac), ASCIIToUTF16(key_exchange));
    }
  }

  ChromeSSLHostStateDelegate* delegate =
       ChromeSSLHostStateDelegateFactory::GetForProfile(profile_);
   DCHECK(delegate);
  show_ssl_decision_revoke_button_ =
      delegate->HasAllowException(url.host()) &&
      visible_security_state.malicious_content_status ==
          security_state::MALICIOUS_CONTENT_STATUS_NONE;
 }
