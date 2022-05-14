void FrameFetchContext::AddResourceTiming(const ResourceTimingInfo& info) {
  if (!document_)
    return;
  LocalFrame* frame = document_->GetFrame();
  if (!frame)
    return;

  if (info.IsMainResource()) {
    DCHECK(frame->Owner());
     frame->Owner()->AddResourceTiming(info);
    frame->DidSendResourceTimingInfoToParent();
     return;
   }
 
  DOMWindowPerformance::performance(*document_->domWindow())
      ->GenerateAndAddResourceTiming(info);
}
