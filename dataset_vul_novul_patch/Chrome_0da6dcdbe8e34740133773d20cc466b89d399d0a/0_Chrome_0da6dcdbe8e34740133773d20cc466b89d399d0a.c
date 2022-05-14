void XSSAuditor::Init(Document* document,
                      XSSAuditorDelegate* auditor_delegate) {
  DCHECK(IsMainThread());
  if (state_ != kUninitialized)
    return;
  state_ = kFilteringTokens;

  if (Settings* settings = document->GetSettings())
    is_enabled_ = settings->GetXSSAuditorEnabled();

  if (!is_enabled_)
    return;

  document_url_ = document->Url().Copy();

  if (!document->GetFrame()) {
    is_enabled_ = false;
    return;
  }

  if (document_url_.IsEmpty()) {
    is_enabled_ = false;
    return;
  }

  if (document_url_.ProtocolIsData()) {
    is_enabled_ = false;
    return;
  }

  if (document->Encoding().IsValid())
    encoding_ = document->Encoding();

  if (DocumentLoader* document_loader =
          document->GetFrame()->Loader().GetDocumentLoader()) {
    const AtomicString& header_value =
        document_loader->GetResponse().HttpHeaderField(
            HTTPNames::X_XSS_Protection);
    String error_details;
    unsigned error_position = 0;
    String report_url;
    KURL xss_protection_report_url;

    ReflectedXSSDisposition xss_protection_header = ParseXSSProtectionHeader(
        header_value, error_details, error_position, report_url);

    if (xss_protection_header == kAllowReflectedXSS)
      UseCounter::Count(*document, WebFeature::kXSSAuditorDisabled);
    else if (xss_protection_header == kFilterReflectedXSS)
      UseCounter::Count(*document, WebFeature::kXSSAuditorEnabledFilter);
    else if (xss_protection_header == kBlockReflectedXSS)
      UseCounter::Count(*document, WebFeature::kXSSAuditorEnabledBlock);
    else if (xss_protection_header == kReflectedXSSInvalid)
      UseCounter::Count(*document, WebFeature::kXSSAuditorInvalid);

    did_send_valid_xss_protection_header_ =
        xss_protection_header != kReflectedXSSUnset &&
        xss_protection_header != kReflectedXSSInvalid;
    if ((xss_protection_header == kFilterReflectedXSS ||
          xss_protection_header == kBlockReflectedXSS) &&
         !report_url.IsEmpty()) {
       xss_protection_report_url = document->CompleteURL(report_url);
      if (!SecurityOrigin::Create(xss_protection_report_url)
               ->IsSameSchemeHostPort(document->GetSecurityOrigin())) {
        error_details =
            "reporting URL is not same scheme, host, and port as page";
        xss_protection_header = kReflectedXSSInvalid;
        xss_protection_report_url = KURL();
      }
       if (MixedContentChecker::IsMixedContent(document->GetSecurityOrigin(),
                                               xss_protection_report_url)) {
         error_details = "insecure reporting URL for secure page";
        xss_protection_header = kReflectedXSSInvalid;
        xss_protection_report_url = KURL();
      }
    }
    if (xss_protection_header == kReflectedXSSInvalid) {
      document->AddConsoleMessage(ConsoleMessage::Create(
          kSecurityMessageSource, kErrorMessageLevel,
          "Error parsing header X-XSS-Protection: " + header_value + ": " +
              error_details + " at character position " +
              String::Format("%u", error_position) +
              ". The default protections will be applied."));
    }

    xss_protection_ = xss_protection_header;
    if (xss_protection_ == kReflectedXSSInvalid ||
        xss_protection_ == kReflectedXSSUnset) {
      xss_protection_ = kBlockReflectedXSS;
    }

    if (auditor_delegate)
      auditor_delegate->SetReportURL(xss_protection_report_url.Copy());

    EncodedFormData* http_body = document_loader->GetRequest().HttpBody();
    if (http_body && !http_body->IsEmpty())
      http_body_as_string_ = http_body->FlattenToString();
  }

  SetEncoding(encoding_);
}
