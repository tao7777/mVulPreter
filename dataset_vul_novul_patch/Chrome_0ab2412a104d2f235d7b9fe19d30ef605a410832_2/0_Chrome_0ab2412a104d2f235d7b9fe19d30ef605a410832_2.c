void DocumentLoader::DidInstallNewDocument(Document* document) {
void DocumentLoader::DidInstallNewDocument(Document* document,
                                           InstallNewDocumentReason reason) {
   document->SetReadyState(Document::kLoading);
  if (content_security_policy_) {
    document->InitContentSecurityPolicy(content_security_policy_.Release());
  }
 
   if (history_item_ && IsBackForwardLoadType(load_type_))
     document->SetStateForNewFormElements(history_item_->GetDocumentState());

  String suborigin_header = response_.HttpHeaderField(HTTPNames::Suborigin);
  if (!suborigin_header.IsNull()) {
    Vector<String> messages;
    Suborigin suborigin;
    if (ParseSuboriginHeader(suborigin_header, &suborigin, messages))
      document->EnforceSuborigin(suborigin);

    for (auto& message : messages) {
      document->AddConsoleMessage(
          ConsoleMessage::Create(kSecurityMessageSource, kErrorMessageLevel,
                                 "Error with Suborigin header: " + message));
    }
  }

  document->GetClientHintsPreferences().UpdateFrom(client_hints_preferences_);

  Settings* settings = document->GetSettings();
  fetcher_->SetImagesEnabled(settings->GetImagesEnabled());
  fetcher_->SetAutoLoadImages(settings->GetLoadsImagesAutomatically());

  const AtomicString& dns_prefetch_control =
      response_.HttpHeaderField(HTTPNames::X_DNS_Prefetch_Control);
  if (!dns_prefetch_control.IsEmpty())
    document->ParseDNSPrefetchControlHeader(dns_prefetch_control);

  String header_content_language =
      response_.HttpHeaderField(HTTPNames::Content_Language);
  if (!header_content_language.IsEmpty()) {
    size_t comma_index = header_content_language.find(',');
    header_content_language.Truncate(comma_index);
    header_content_language =
        header_content_language.StripWhiteSpace(IsHTMLSpace<UChar>);
    if (!header_content_language.IsEmpty())
      document->SetContentLanguage(AtomicString(header_content_language));
  }

  OriginTrialContext::AddTokensFromHeader(
      document, response_.HttpHeaderField(HTTPNames::Origin_Trial));
  String referrer_policy_header =
      response_.HttpHeaderField(HTTPNames::Referrer_Policy);
  if (!referrer_policy_header.IsNull()) {
    UseCounter::Count(*document, WebFeature::kReferrerPolicyHeader);
    document->ParseAndSetReferrerPolicy(referrer_policy_header);
  }

  GetLocalFrameClient().DidCreateNewDocument();
}
