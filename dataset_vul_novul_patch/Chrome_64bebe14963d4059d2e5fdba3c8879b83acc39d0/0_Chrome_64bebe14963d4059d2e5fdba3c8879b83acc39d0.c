void XSSAuditor::init(Document* document, XSSAuditorDelegate* auditorDelegate)
{
    const size_t miniumLengthForSuffixTree = 512; // FIXME: Tune this parameter.
    const int suffixTreeDepth = 5;

    ASSERT(isMainThread());
    if (m_state != Uninitialized)
        return;
    m_state = FilteringTokens;

    if (Settings* settings = document->settings())
        m_isEnabled = settings->xssAuditorEnabled();

    if (!m_isEnabled)
        return;

    m_documentURL = document->url().copy();

    if (!document->frame()) {
        m_isEnabled = false;
        return;
    }

    if (m_documentURL.isEmpty()) {
        m_isEnabled = false;
        return;
    }

    if (m_documentURL.protocolIsData()) {
        m_isEnabled = false;
        return;
    }

    if (document->encoding().isValid())
        m_encoding = document->encoding();

    m_decodedURL = fullyDecodeString(m_documentURL.string(), m_encoding);
    if (m_decodedURL.find(isRequiredForInjection) == kNotFound)
        m_decodedURL = String();

    String httpBodyAsString;
    if (DocumentLoader* documentLoader = document->frame()->loader().documentLoader()) {
        DEFINE_STATIC_LOCAL(const AtomicString, XSSProtectionHeader, ("X-XSS-Protection", AtomicString::ConstructFromLiteral));
        const AtomicString& headerValue = documentLoader->response().httpHeaderField(XSSProtectionHeader);
        String errorDetails;
        unsigned errorPosition = 0;
        String reportURL;
        KURL xssProtectionReportURL;

        ReflectedXSSDisposition xssProtectionHeader = parseXSSProtectionHeader(headerValue, errorDetails, errorPosition, reportURL);
        m_didSendValidXSSProtectionHeader = xssProtectionHeader != ReflectedXSSUnset && xssProtectionHeader != ReflectedXSSInvalid;
        if ((xssProtectionHeader == FilterReflectedXSS || xssProtectionHeader == BlockReflectedXSS) && !reportURL.isEmpty()) {
            xssProtectionReportURL = document->completeURL(reportURL);
            if (MixedContentChecker::isMixedContent(document->securityOrigin(), xssProtectionReportURL)) {
                errorDetails = "insecure reporting URL for secure page";
                xssProtectionHeader = ReflectedXSSInvalid;
                xssProtectionReportURL = KURL();
            }
        }
        if (xssProtectionHeader == ReflectedXSSInvalid)
            document->addConsoleMessage(SecurityMessageSource, ErrorMessageLevel, "Error parsing header X-XSS-Protection: " + headerValue + ": "  + errorDetails + " at character position " + String::format("%u", errorPosition) + ". The default protections will be applied.");

        ReflectedXSSDisposition cspHeader = document->contentSecurityPolicy()->reflectedXSSDisposition();
        m_didSendValidCSPHeader = cspHeader != ReflectedXSSUnset && cspHeader != ReflectedXSSInvalid;

        m_xssProtection = combineXSSProtectionHeaderAndCSP(xssProtectionHeader, cspHeader);
         if (auditorDelegate)
             auditorDelegate->setReportURL(xssProtectionReportURL.copy());
        FormData* httpBody = documentLoader->request().httpBody();
         if (httpBody && !httpBody->isEmpty()) {
             httpBodyAsString = httpBody->flattenToString();
             if (!httpBodyAsString.isEmpty()) {
                m_decodedHTTPBody = fullyDecodeString(httpBodyAsString, m_encoding);
                if (m_decodedHTTPBody.find(isRequiredForInjection) == kNotFound)
                    m_decodedHTTPBody = String();
                if (m_decodedHTTPBody.length() >= miniumLengthForSuffixTree)
                    m_decodedHTTPBodySuffixTree = adoptPtr(new SuffixTree<ASCIICodebook>(m_decodedHTTPBody, suffixTreeDepth));
            }
        }
    }

    if (m_decodedURL.isEmpty() && m_decodedHTTPBody.isEmpty()) {
        m_isEnabled = false;
        return;
    }
}
