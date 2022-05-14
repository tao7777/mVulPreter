void ContentSecurityPolicy::postViolationReport(
    const SecurityPolicyViolationEventInit& violationData,
    LocalFrame* contextFrame,
    const Vector<String>& reportEndpoints) {
  Document* document =
      contextFrame ? contextFrame->document() : this->document();
  if (!document)
    return;

  //
  // TODO(mkwst): This justification is BS. Insecure reports are mixed content,
  // let's kill them. https://crbug.com/695363
 
   std::unique_ptr<JSONObject> cspReport = JSONObject::create();
   cspReport->setString("document-uri", violationData.documentURI());
  cspReport->setString("referrer", violationData.referrer());
  cspReport->setString("violated-directive", violationData.violatedDirective());
  cspReport->setString("effective-directive",
                       violationData.effectiveDirective());
  cspReport->setString("original-policy", violationData.originalPolicy());
  cspReport->setString("disposition", violationData.disposition());
  cspReport->setString("blocked-uri", violationData.blockedURI());
  if (violationData.lineNumber())
    cspReport->setInteger("line-number", violationData.lineNumber());
  if (violationData.columnNumber())
    cspReport->setInteger("column-number", violationData.columnNumber());
  if (!violationData.sourceFile().isEmpty())
    cspReport->setString("source-file", violationData.sourceFile());
  cspReport->setInteger("status-code", violationData.statusCode());

  if (experimentalFeaturesEnabled())
    cspReport->setString("sample", violationData.sample());

  std::unique_ptr<JSONObject> reportObject = JSONObject::create();
  reportObject->setObject("csp-report", std::move(cspReport));
  String stringifiedReport = reportObject->toJSONString();

  if (shouldSendViolationReport(stringifiedReport)) {
    didSendViolationReport(stringifiedReport);

    RefPtr<EncodedFormData> report =
        EncodedFormData::create(stringifiedReport.utf8());

    LocalFrame* frame = document->frame();
    if (!frame)
      return;

    for (const String& endpoint : reportEndpoints) {
      DCHECK(!contextFrame || !m_executionContext);
      DCHECK(!contextFrame ||
             getDirectiveType(violationData.effectiveDirective()) ==
                 DirectiveType::FrameAncestors);
      KURL url =
          contextFrame
              ? frame->document()->completeURLWithOverride(
                    endpoint, KURL(ParsedURLString, violationData.blockedURI()))
              : completeURL(endpoint);
      PingLoader::sendViolationReport(
          frame, url, report, PingLoader::ContentSecurityPolicyViolationReport);
    }
  }
}
