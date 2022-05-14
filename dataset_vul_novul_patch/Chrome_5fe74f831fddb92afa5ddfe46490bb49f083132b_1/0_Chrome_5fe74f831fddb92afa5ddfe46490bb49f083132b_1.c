bool FrameFetchContext::UpdateTimingInfoForIFrameNavigation(
    ResourceTimingInfo* info) {
  if (IsDetached())
    return false;

  if (!GetFrame()->Owner())
    return false;
   if (!GetFrame()->should_send_resource_timing_info_to_parent())
     return false;
  // location may have been changed after initial navigation,
  if (MasterDocumentLoader()->LoadType() == WebFrameLoadType::kBackForward) {
    // ...and do not report subsequent navigations in the iframe too.
    GetFrame()->SetShouldSendResourceTimingInfoToParent(false);
     return false;
  }
   return true;
 }
