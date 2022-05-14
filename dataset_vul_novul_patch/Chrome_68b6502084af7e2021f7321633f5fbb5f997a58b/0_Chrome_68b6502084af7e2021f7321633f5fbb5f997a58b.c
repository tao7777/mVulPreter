SSLErrorInfo SSLErrorInfo::CreateError(ErrorType error_type,
                                       net::X509Certificate* cert,
                                       const GURL& request_url) {
  string16 title, details, short_description;
  std::vector<string16> extra_info;
  switch (error_type) {
    case CERT_COMMON_NAME_INVALID: {
      title =
          l10n_util::GetStringUTF16(IDS_CERT_ERROR_COMMON_NAME_INVALID_TITLE);
      std::vector<std::string> dns_names;
      cert->GetDNSNames(&dns_names);
      DCHECK(!dns_names.empty());
      size_t i = 0;
      for (; i < dns_names.size(); ++i) {
        if (dns_names[i] == cert->subject().common_name)
          break;
      }
      if (i == dns_names.size())
        i = 0;
       details =
           l10n_util::GetStringFUTF16(IDS_CERT_ERROR_COMMON_NAME_INVALID_DETAILS,
                                      UTF8ToUTF16(request_url.host()),
                                     net::EscapeForHTML(
                                         UTF8ToUTF16(dns_names[i])),
                                      UTF8ToUTF16(request_url.host()));
       short_description = l10n_util::GetStringUTF16(
           IDS_CERT_ERROR_COMMON_NAME_INVALID_DESCRIPTION);
      extra_info.push_back(
          l10n_util::GetStringUTF16(IDS_CERT_ERROR_EXTRA_INFO_1));
       extra_info.push_back(
           l10n_util::GetStringFUTF16(
               IDS_CERT_ERROR_COMMON_NAME_INVALID_EXTRA_INFO_2,
              net::EscapeForHTML(UTF8ToUTF16(cert->subject().common_name)),
               UTF8ToUTF16(request_url.host())));
       break;
     }
    case CERT_DATE_INVALID:
      extra_info.push_back(
          l10n_util::GetStringUTF16(IDS_CERT_ERROR_EXTRA_INFO_1));
      if (cert->HasExpired()) {
        title = l10n_util::GetStringUTF16(IDS_CERT_ERROR_EXPIRED_TITLE);
        details = l10n_util::GetStringFUTF16(
            IDS_CERT_ERROR_EXPIRED_DETAILS,
            UTF8ToUTF16(request_url.host()),
            UTF8ToUTF16(request_url.host()),
            base::TimeFormatFriendlyDateAndTime(base::Time::Now()));
        short_description =
            l10n_util::GetStringUTF16(IDS_CERT_ERROR_EXPIRED_DESCRIPTION);
        extra_info.push_back(l10n_util::GetStringUTF16(
            IDS_CERT_ERROR_EXPIRED_DETAILS_EXTRA_INFO_2));
      } else {
        title = l10n_util::GetStringUTF16(IDS_CERT_ERROR_NOT_YET_VALID_TITLE);
        details = l10n_util::GetStringFUTF16(
            IDS_CERT_ERROR_NOT_YET_VALID_DETAILS,
            UTF8ToUTF16(request_url.host()),
            UTF8ToUTF16(request_url.host()),
            base::TimeFormatFriendlyDateAndTime(base::Time::Now()));
        short_description =
            l10n_util::GetStringUTF16(IDS_CERT_ERROR_NOT_YET_VALID_DESCRIPTION);
        extra_info.push_back(
            l10n_util::GetStringUTF16(
                IDS_CERT_ERROR_NOT_YET_VALID_DETAILS_EXTRA_INFO_2));
      }
      break;
    case CERT_AUTHORITY_INVALID:
      title = l10n_util::GetStringUTF16(IDS_CERT_ERROR_AUTHORITY_INVALID_TITLE);
      details = l10n_util::GetStringFUTF16(
          IDS_CERT_ERROR_AUTHORITY_INVALID_DETAILS,
          UTF8ToUTF16(request_url.host()));
      short_description = l10n_util::GetStringUTF16(
          IDS_CERT_ERROR_AUTHORITY_INVALID_DESCRIPTION);
      extra_info.push_back(
          l10n_util::GetStringUTF16(IDS_CERT_ERROR_EXTRA_INFO_1));
      extra_info.push_back(l10n_util::GetStringFUTF16(
          IDS_CERT_ERROR_AUTHORITY_INVALID_EXTRA_INFO_2,
          UTF8ToUTF16(request_url.host()),
          UTF8ToUTF16(request_url.host())));
      extra_info.push_back(l10n_util::GetStringUTF16(
          IDS_CERT_ERROR_AUTHORITY_INVALID_EXTRA_INFO_3));
      break;
    case CERT_CONTAINS_ERRORS:
      title = l10n_util::GetStringUTF16(IDS_CERT_ERROR_CONTAINS_ERRORS_TITLE);
      details = l10n_util::GetStringFUTF16(
          IDS_CERT_ERROR_CONTAINS_ERRORS_DETAILS,
          UTF8ToUTF16(request_url.host()));
      short_description =
          l10n_util::GetStringUTF16(IDS_CERT_ERROR_CONTAINS_ERRORS_DESCRIPTION);
      extra_info.push_back(
          l10n_util::GetStringFUTF16(IDS_CERT_ERROR_EXTRA_INFO_1,
                                     UTF8ToUTF16(request_url.host())));
      extra_info.push_back(l10n_util::GetStringUTF16(
          IDS_CERT_ERROR_CONTAINS_ERRORS_EXTRA_INFO_2));
      break;
    case CERT_NO_REVOCATION_MECHANISM:
      title = l10n_util::GetStringUTF16(
          IDS_CERT_ERROR_NO_REVOCATION_MECHANISM_TITLE);
      details = l10n_util::GetStringUTF16(
          IDS_CERT_ERROR_NO_REVOCATION_MECHANISM_DETAILS);
      short_description = l10n_util::GetStringUTF16(
          IDS_CERT_ERROR_NO_REVOCATION_MECHANISM_DESCRIPTION);
      break;
    case CERT_UNABLE_TO_CHECK_REVOCATION:
      title = l10n_util::GetStringUTF16(
          IDS_CERT_ERROR_UNABLE_TO_CHECK_REVOCATION_TITLE);
      details = l10n_util::GetStringUTF16(
          IDS_CERT_ERROR_UNABLE_TO_CHECK_REVOCATION_DETAILS);
      short_description = l10n_util::GetStringUTF16(
          IDS_CERT_ERROR_UNABLE_TO_CHECK_REVOCATION_DESCRIPTION);
      break;
    case CERT_REVOKED:
      title = l10n_util::GetStringUTF16(IDS_CERT_ERROR_REVOKED_CERT_TITLE);
      details = l10n_util::GetStringFUTF16(IDS_CERT_ERROR_REVOKED_CERT_DETAILS,
                                           UTF8ToUTF16(request_url.host()));
      short_description =
          l10n_util::GetStringUTF16(IDS_CERT_ERROR_REVOKED_CERT_DESCRIPTION);
      extra_info.push_back(
          l10n_util::GetStringUTF16(IDS_CERT_ERROR_EXTRA_INFO_1));
      extra_info.push_back(
          l10n_util::GetStringUTF16(IDS_CERT_ERROR_REVOKED_CERT_EXTRA_INFO_2));
      break;
    case CERT_INVALID:
      title = l10n_util::GetStringUTF16(IDS_CERT_ERROR_INVALID_CERT_TITLE);
      details = l10n_util::GetStringFUTF16(
          IDS_CERT_ERROR_INVALID_CERT_DETAILS,
          UTF8ToUTF16(request_url.host()));
      short_description =
          l10n_util::GetStringUTF16(IDS_CERT_ERROR_INVALID_CERT_DESCRIPTION);
      extra_info.push_back(
          l10n_util::GetStringUTF16(IDS_CERT_ERROR_EXTRA_INFO_1));
      extra_info.push_back(l10n_util::GetStringUTF16(
          IDS_CERT_ERROR_INVALID_CERT_EXTRA_INFO_2));
      break;
    case CERT_WEAK_SIGNATURE_ALGORITHM:
      title = l10n_util::GetStringUTF16(
          IDS_CERT_ERROR_WEAK_SIGNATURE_ALGORITHM_TITLE);
      details = l10n_util::GetStringFUTF16(
          IDS_CERT_ERROR_WEAK_SIGNATURE_ALGORITHM_DETAILS,
          UTF8ToUTF16(request_url.host()));
      short_description = l10n_util::GetStringUTF16(
          IDS_CERT_ERROR_WEAK_SIGNATURE_ALGORITHM_DESCRIPTION);
      extra_info.push_back(
          l10n_util::GetStringUTF16(IDS_CERT_ERROR_EXTRA_INFO_1));
      extra_info.push_back(
          l10n_util::GetStringUTF16(
              IDS_CERT_ERROR_WEAK_SIGNATURE_ALGORITHM_EXTRA_INFO_2));
      break;
    case CERT_WEAK_KEY:
      title = l10n_util::GetStringUTF16(IDS_CERT_ERROR_WEAK_KEY_TITLE);
      details = l10n_util::GetStringFUTF16(
          IDS_CERT_ERROR_WEAK_KEY_DETAILS, UTF8ToUTF16(request_url.host()));
      short_description = l10n_util::GetStringUTF16(
          IDS_CERT_ERROR_WEAK_KEY_DESCRIPTION);
      extra_info.push_back(
          l10n_util::GetStringUTF16(IDS_CERT_ERROR_EXTRA_INFO_1));
      extra_info.push_back(
          l10n_util::GetStringUTF16(
              IDS_CERT_ERROR_WEAK_KEY_EXTRA_INFO_2));
      break;
    case UNKNOWN:
      title = l10n_util::GetStringUTF16(IDS_CERT_ERROR_UNKNOWN_ERROR_TITLE);
      details = l10n_util::GetStringUTF16(IDS_CERT_ERROR_UNKNOWN_ERROR_DETAILS);
      short_description =
          l10n_util::GetStringUTF16(IDS_CERT_ERROR_UNKNOWN_ERROR_DESCRIPTION);
      break;
    default:
      NOTREACHED();
  }
  return SSLErrorInfo(title, details, short_description, extra_info);
}
