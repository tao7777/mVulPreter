static void gatherSecurityPolicyViolationEventData(
    SecurityPolicyViolationEventInit& init,
    ExecutionContext* context,
    const String& directiveText,
    const ContentSecurityPolicy::DirectiveType& effectiveType,
    const KURL& blockedURL,
    const String& header,
    RedirectStatus redirectStatus,
    ContentSecurityPolicyHeaderType headerType,
    ContentSecurityPolicy::ViolationType violationType,
    int contextLine,
    const String& scriptSource) {
  if (effectiveType == ContentSecurityPolicy::DirectiveType::FrameAncestors) {
    String strippedURL = stripURLForUseInReport(
        context, blockedURL, RedirectStatus::NoRedirect,
        ContentSecurityPolicy::DirectiveType::DefaultSrc);
    init.setDocumentURI(strippedURL);
    init.setBlockedURI(strippedURL);
   } else {
    String strippedURL = stripURLForUseInReport(
        context, context->url(), RedirectStatus::NoRedirect,
        ContentSecurityPolicy::DirectiveType::DefaultSrc);
    init.setDocumentURI(strippedURL);
     switch (violationType) {
       case ContentSecurityPolicy::InlineViolation:
         init.setBlockedURI("inline");
        break;
      case ContentSecurityPolicy::EvalViolation:
        init.setBlockedURI("eval");
        break;
      case ContentSecurityPolicy::URLViolation:
        init.setBlockedURI(stripURLForUseInReport(
            context, blockedURL, redirectStatus, effectiveType));
        break;
    }
  }

  String effectiveDirective =
      ContentSecurityPolicy::getDirectiveName(effectiveType);
  init.setViolatedDirective(effectiveDirective);
  init.setEffectiveDirective(effectiveDirective);
  init.setOriginalPolicy(header);
  init.setDisposition(headerType == ContentSecurityPolicyHeaderTypeEnforce
                          ? "enforce"
                          : "report");
  init.setSourceFile(String());
  init.setLineNumber(contextLine);
  init.setColumnNumber(0);
  init.setStatusCode(0);

  if (context->isDocument()) {
    Document* document = toDocument(context);
    DCHECK(document);
    init.setReferrer(document->referrer());
    if (!SecurityOrigin::isSecure(context->url()) && document->loader())
      init.setStatusCode(document->loader()->response().httpStatusCode());
  }

  std::unique_ptr<SourceLocation> location = SourceLocation::capture(context);
  if (location->lineNumber()) {
    KURL source = KURL(ParsedURLString, location->url());
    init.setSourceFile(
        stripURLForUseInReport(context, source, redirectStatus, effectiveType));
    init.setLineNumber(location->lineNumber());
    init.setColumnNumber(location->columnNumber());
  }

  if (!scriptSource.isEmpty())
    init.setSample(scriptSource.stripWhiteSpace().left(40));
}
