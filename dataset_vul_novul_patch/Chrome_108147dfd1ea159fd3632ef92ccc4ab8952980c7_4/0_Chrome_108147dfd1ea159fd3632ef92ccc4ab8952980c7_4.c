void DocumentLoader::InstallNewDocument(
    const KURL& url,
    Document* owner_document,
    WebGlobalObjectReusePolicy global_object_reuse_policy,
    const AtomicString& mime_type,
    const AtomicString& encoding,
    InstallNewDocumentReason reason,
    ParserSynchronizationPolicy parsing_policy,
    const KURL& overriding_url) {
  DCHECK(!frame_->GetDocument() || !frame_->GetDocument()->IsActive());
  DCHECK_EQ(frame_->Tree().ChildCount(), 0u);
  if (GetFrameLoader().StateMachine()->IsDisplayingInitialEmptyDocument()) {
    GetFrameLoader().StateMachine()->AdvanceTo(
        FrameLoaderStateMachine::kCommittedFirstRealLoad);
   }
 
   const SecurityOrigin* previous_security_origin = nullptr;
   if (frame_->GetDocument()) {
     previous_security_origin = frame_->GetDocument()->GetSecurityOrigin();
   }
 
  if (global_object_reuse_policy != WebGlobalObjectReusePolicy::kUseExisting)
    frame_->SetDOMWindow(LocalDOMWindow::Create(*frame_));

  if (reason == InstallNewDocumentReason::kNavigation)
    WillCommitNavigation();

  Document* document = frame_->DomWindow()->InstallNewDocument(
      mime_type,
      DocumentInit::Create()
           .WithDocumentLoader(this)
           .WithURL(url)
           .WithOwnerDocument(owner_document)
          .WithNewRegistrationContext(),
       false);
 
  if (frame_->IsMainFrame())
    frame_->ClearActivation();

  if (frame_->HasReceivedUserGestureBeforeNavigation() !=
      had_sticky_activation_) {
    frame_->SetDocumentHasReceivedUserGestureBeforeNavigation(
        had_sticky_activation_);
    GetLocalFrameClient().SetHasReceivedUserGestureBeforeNavigation(
        had_sticky_activation_);
  }

  if (ShouldClearWindowName(*frame_, previous_security_origin, *document)) {
    frame_->Tree().ExperimentalSetNulledName();
  }
 
   if (!overriding_url.IsEmpty())
     document->SetBaseURLOverride(overriding_url);
  DidInstallNewDocument(document);
 
  if (reason == InstallNewDocumentReason::kNavigation)
    DidCommitNavigation(global_object_reuse_policy);

  if (GetFrameLoader().StateMachine()->CommittedFirstRealDocumentLoad()) {
    if (document->GetSettings()
            ->GetForceTouchEventFeatureDetectionForInspector()) {
      OriginTrialContext::FromOrCreate(document)->AddFeature(
          "ForceTouchEventFeatureDetectionForInspector");
    }
    OriginTrialContext::AddTokensFromHeader(
        document, response_.HttpHeaderField(http_names::kOriginTrial));
  }
  bool stale_while_revalidate_enabled =
      origin_trials::StaleWhileRevalidateEnabled(document);
  fetcher_->SetStaleWhileRevalidateEnabled(stale_while_revalidate_enabled);

  if (stale_while_revalidate_enabled &&
      !RuntimeEnabledFeatures::StaleWhileRevalidateEnabledByRuntimeFlag())
    UseCounter::Count(frame_, WebFeature::kStaleWhileRevalidateEnabled);

  parser_ = document->OpenForNavigation(parsing_policy, mime_type, encoding);

  ScriptableDocumentParser* scriptable_parser =
      parser_->AsScriptableDocumentParser();
  if (scriptable_parser && GetResource()) {
    scriptable_parser->SetInlineScriptCacheHandler(
        ToRawResource(GetResource())->InlineScriptCacheHandler());
  }

  WTF::String feature_policy(
      response_.HttpHeaderField(http_names::kFeaturePolicy));
  MergeFeaturesFromOriginPolicy(feature_policy, request_.GetOriginPolicy());
  document->ApplyFeaturePolicyFromHeader(feature_policy);

  GetFrameLoader().DispatchDidClearDocumentOfWindowObject();
}
