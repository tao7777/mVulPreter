void VRDisplay::BeginPresent() {
  Document* doc = this->GetDocument();
  if (capabilities_->hasExternalDisplay()) {
    ForceExitPresent();
    DOMException* exception = DOMException::Create(
        kInvalidStateError,
        "VR Presentation not implemented for this VRDisplay.");
    while (!pending_present_resolvers_.IsEmpty()) {
      ScriptPromiseResolver* resolver = pending_present_resolvers_.TakeFirst();
      resolver->Reject(exception);
    }
    ReportPresentationResult(
        PresentationResult::kPresentationNotSupportedByDisplay);
    return;
  } else {
    if (layer_.source().isHTMLCanvasElement()) {
    } else {
      DCHECK(layer_.source().isOffscreenCanvas());
      ForceExitPresent();
      DOMException* exception = DOMException::Create(
          kInvalidStateError, "OffscreenCanvas presentation not implemented.");
      while (!pending_present_resolvers_.IsEmpty()) {
        ScriptPromiseResolver* resolver =
            pending_present_resolvers_.TakeFirst();
        resolver->Reject(exception);
      }
      ReportPresentationResult(
          PresentationResult::kPresentationNotSupportedByDisplay);
      return;
    }
  }

  if (doc) {
    Platform::Current()->RecordRapporURL("VR.WebVR.PresentSuccess",
                                         WebURL(doc->Url()));
  }

  is_presenting_ = true;
  ReportPresentationResult(PresentationResult::kSuccess);

  UpdateLayerBounds();

  while (!pending_present_resolvers_.IsEmpty()) {
    ScriptPromiseResolver* resolver = pending_present_resolvers_.TakeFirst();
     resolver->Resolve();
   }
   OnPresentChange();
 }
