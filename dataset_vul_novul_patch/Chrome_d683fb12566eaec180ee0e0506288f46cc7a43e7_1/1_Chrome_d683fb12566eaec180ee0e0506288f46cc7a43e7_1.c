void DocumentLoader::DidInstallNewDocument(Document* document) {
   document->SetReadyState(Document::kLoading);
   if (content_security_policy_) {
    document->InitContentSecurityPolicy(content_security_policy_.Release());
   }
 
   if (history_item_ && IsBackForwardLoadType(load_type_))
    document->SetStateForNewFormElements(history_item_->GetDocumentState());

  DCHECK(document->GetFrame());
  document->GetFrame()->GetClientHintsPreferences().UpdateFrom(
      client_hints_preferences_);

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

  String referrer_policy_header =
      response_.HttpHeaderField(HTTPNames::Referrer_Policy);
  if (!referrer_policy_header.IsNull()) {
    UseCounter::Count(*document, WebFeature::kReferrerPolicyHeader);
    document->ParseAndSetReferrerPolicy(referrer_policy_header);
  }

  if (response_.IsSignedExchangeInnerResponse())
    UseCounter::Count(*document, WebFeature::kSignedExchangeInnerResponse);

  GetLocalFrameClient().DidCreateNewDocument();
}
