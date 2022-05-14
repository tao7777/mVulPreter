void DocumentLoader::InstallNewDocument(
    const KURL& url,
    Document* owner_document,
    bool should_reuse_default_view,
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

  SecurityOrigin* previous_security_origin = nullptr;
  if (frame_->GetDocument())
    previous_security_origin = frame_->GetDocument()->GetSecurityOrigin();

  if (!should_reuse_default_view)
    frame_->SetDOMWindow(LocalDOMWindow::Create(*frame_));

  bool user_gesture_bit_set = frame_->HasReceivedUserGesture() ||
                              frame_->HasReceivedUserGestureBeforeNavigation();

  if (reason == InstallNewDocumentReason::kNavigation)
    WillCommitNavigation();

  Document* document = frame_->DomWindow()->InstallNewDocument(
      mime_type,
      DocumentInit::Create()
          .WithFrame(frame_)
          .WithURL(url)
          .WithOwnerDocument(owner_document)
          .WithNewRegistrationContext(),
      false);

  if (user_gesture_bit_set) {
    frame_->SetDocumentHasReceivedUserGestureBeforeNavigation(
        ShouldPersistUserGestureValue(previous_security_origin,
                                      document->GetSecurityOrigin()));

    if (frame_->IsMainFrame())
      frame_->ClearDocumentHasReceivedUserGesture();
  }

  if (ShouldClearWindowName(*frame_, previous_security_origin, *document)) {
    frame_->Tree().ExperimentalSetNulledName();
  }

   frame_->GetPage()->GetChromeClient().InstallSupplements(*frame_);
   if (!overriding_url.IsEmpty())
     document->SetBaseURLOverride(overriding_url);
  DidInstallNewDocument(document);
 
  if (reason == InstallNewDocumentReason::kNavigation)
    DidCommitNavigation();

  writer_ =
      DocumentWriter::Create(document, parsing_policy, mime_type, encoding);

  document->SetFeaturePolicy(
      RuntimeEnabledFeatures::FeaturePolicyExperimentalFeaturesEnabled()
          ? response_.HttpHeaderField(HTTPNames::Feature_Policy)
          : g_empty_string);

  GetFrameLoader().DispatchDidClearDocumentOfWindowObject();
}
