 ScriptPromise VRDisplay::requestPresent(ScriptState* script_state,
                                         const HeapVector<VRLayer>& layers) {
   ExecutionContext* execution_context = ExecutionContext::From(script_state);
   UseCounter::Count(execution_context, UseCounter::kVRRequestPresent);
   if (!execution_context->IsSecureContext()) {
    UseCounter::Count(execution_context,
                      UseCounter::kVRRequestPresentInsecureOrigin);
  }

  ReportPresentationResult(PresentationResult::kRequested);

  ScriptPromiseResolver* resolver = ScriptPromiseResolver::Create(script_state);
  ScriptPromise promise = resolver->Promise();

  if (!capabilities_->canPresent()) {
    DOMException* exception =
        DOMException::Create(kInvalidStateError, "VRDisplay cannot present.");
    resolver->Reject(exception);
    ReportPresentationResult(PresentationResult::kVRDisplayCannotPresent);
    return promise;
  }

  bool first_present = !is_presenting_;

  if (first_present && !UserGestureIndicator::UtilizeUserGesture() &&
      !in_display_activate_) {
    DOMException* exception = DOMException::Create(
        kInvalidStateError, "API can only be initiated by a user gesture.");
    resolver->Reject(exception);
    ReportPresentationResult(PresentationResult::kNotInitiatedByUserGesture);
    return promise;
  }

  if (layers.size() == 0 || layers.size() > capabilities_->maxLayers()) {
    ForceExitPresent();
    DOMException* exception =
        DOMException::Create(kInvalidStateError, "Invalid number of layers.");
    resolver->Reject(exception);
    ReportPresentationResult(PresentationResult::kInvalidNumberOfLayers);
    return promise;
  }

  if (layers[0].source().isNull()) {
    ForceExitPresent();
    DOMException* exception =
        DOMException::Create(kInvalidStateError, "Invalid layer source.");
    resolver->Reject(exception);
    ReportPresentationResult(PresentationResult::kInvalidLayerSource);
    return promise;
  }
  layer_ = layers[0];

  CanvasRenderingContext* rendering_context;
  if (layer_.source().isHTMLCanvasElement()) {
    rendering_context =
        layer_.source().getAsHTMLCanvasElement()->RenderingContext();
  } else {
    DCHECK(layer_.source().isOffscreenCanvas());
    rendering_context =
        layer_.source().getAsOffscreenCanvas()->RenderingContext();
  }

  if (!rendering_context || !rendering_context->Is3d()) {
    ForceExitPresent();
    DOMException* exception = DOMException::Create(
        kInvalidStateError, "Layer source must have a WebGLRenderingContext");
    resolver->Reject(exception);
    ReportPresentationResult(
        PresentationResult::kLayerSourceMissingWebGLContext);
    return promise;
  }

  rendering_context_ = ToWebGLRenderingContextBase(rendering_context);
  context_gl_ = rendering_context_->ContextGL();

  if ((layer_.leftBounds().size() != 0 && layer_.leftBounds().size() != 4) ||
      (layer_.rightBounds().size() != 0 && layer_.rightBounds().size() != 4)) {
    ForceExitPresent();
    DOMException* exception = DOMException::Create(
        kInvalidStateError,
        "Layer bounds must either be an empty array or have 4 values");
    resolver->Reject(exception);
    ReportPresentationResult(PresentationResult::kInvalidLayerBounds);
    return promise;
  }

  if (!pending_present_resolvers_.IsEmpty()) {
    pending_present_resolvers_.push_back(resolver);
  } else if (first_present) {
    bool secure_context =
        ExecutionContext::From(script_state)->IsSecureContext();
    if (!display_) {
      ForceExitPresent();
      DOMException* exception = DOMException::Create(
          kInvalidStateError, "The service is no longer active.");
      resolver->Reject(exception);
      return promise;
    }

    pending_present_resolvers_.push_back(resolver);
    submit_frame_client_binding_.Close();
    display_->RequestPresent(
        secure_context,
        submit_frame_client_binding_.CreateInterfacePtrAndBind(),
        ConvertToBaseCallback(
            WTF::Bind(&VRDisplay::OnPresentComplete, WrapPersistent(this))));
  } else {
    UpdateLayerBounds();
    resolver->Resolve();
    ReportPresentationResult(PresentationResult::kSuccessAlreadyPresenting);
  }

  return promise;
}
