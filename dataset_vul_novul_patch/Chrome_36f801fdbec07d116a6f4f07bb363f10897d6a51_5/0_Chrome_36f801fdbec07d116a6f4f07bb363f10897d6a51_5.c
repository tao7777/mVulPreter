void FrameLoader::Load(const FrameLoadRequest& passed_request,
                       FrameLoadType frame_load_type,
                       HistoryItem* history_item,
                       HistoryLoadType history_load_type) {
  DCHECK(frame_->GetDocument());

  if (IsBackForwardLoadType(frame_load_type) && !frame_->IsNavigationAllowed())
    return;

  if (in_stop_all_loaders_)
    return;

  FrameLoadRequest request(passed_request);
  request.GetResourceRequest().SetHasUserGesture(
      Frame::HasTransientUserActivation(frame_));

  if (!PrepareRequestForThisFrame(request))
    return;

  Frame* target_frame = request.Form()
                            ? nullptr
                            : frame_->FindFrameForNavigation(
                                  AtomicString(request.FrameName()), *frame_,
                                  request.GetResourceRequest().Url());

  NavigationPolicy policy = NavigationPolicyForRequest(request);
  if (target_frame && target_frame != frame_ &&
      ShouldNavigateTargetFrame(policy)) {
    if (target_frame->IsLocalFrame() &&
        !ToLocalFrame(target_frame)->IsNavigationAllowed()) {
      return;
    }

    bool was_in_same_page = target_frame->GetPage() == frame_->GetPage();

    request.SetFrameName("_self");
     target_frame->Navigate(request);
     Page* page = target_frame->GetPage();
     if (!was_in_same_page && page)
      page->GetChromeClient().Focus(nullptr);
     return;
   }
 
  SetReferrerForFrameRequest(request);

  if (!target_frame && !request.FrameName().IsEmpty()) {
    if (policy == kNavigationPolicyDownload) {
      Client()->DownloadURL(request.GetResourceRequest(), String());
      return;  // Navigation/download will be handled by the client.
    } else if (ShouldNavigateTargetFrame(policy)) {
      request.GetResourceRequest().SetFrameType(
          network::mojom::RequestContextFrameType::kAuxiliary);
      CreateWindowForRequest(request, *frame_, policy);
      return;  // Navigation will be handled by the new frame/window.
    }
  }

  if (!frame_->IsNavigationAllowed())
    return;

  const KURL& url = request.GetResourceRequest().Url();
  FrameLoadType new_load_type = (frame_load_type == kFrameLoadTypeStandard)
                                    ? DetermineFrameLoadType(request)
                                    : frame_load_type;
  bool same_document_history_navigation =
      IsBackForwardLoadType(new_load_type) &&
      history_load_type == kHistorySameDocumentLoad;
  bool same_document_navigation =
      policy == kNavigationPolicyCurrentTab &&
      ShouldPerformFragmentNavigation(request.Form(),
                                      request.GetResourceRequest().HttpMethod(),
                                      new_load_type, url);

  if (same_document_history_navigation || same_document_navigation) {
    DCHECK(history_item || !same_document_history_navigation);
    scoped_refptr<SerializedScriptValue> state_object =
        same_document_history_navigation ? history_item->StateObject()
                                         : nullptr;

    if (!same_document_history_navigation) {
      document_loader_->SetNavigationType(DetermineNavigationType(
          new_load_type, false, request.TriggeringEvent()));
      if (ShouldTreatURLAsSameAsCurrent(url))
        new_load_type = kFrameLoadTypeReplaceCurrentItem;
    }

    LoadInSameDocument(url, state_object, new_load_type, history_item,
                       request.ClientRedirect(), request.OriginDocument());
    return;
  }

  if (request.GetResourceRequest().IsSameDocumentNavigation())
    return;

  StartLoad(request, new_load_type, policy, history_item);
}
