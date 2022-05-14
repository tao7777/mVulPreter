bool FrameFetchContext::UpdateTimingInfoForIFrameNavigation(
    ResourceTimingInfo* info) {
  if (IsDetached())
    return false;

  if (!GetFrame()->Owner())
    return false;
   if (!GetFrame()->should_send_resource_timing_info_to_parent())
     return false;
  if (MasterDocumentLoader()->LoadType() == WebFrameLoadType::kBackForward)
     return false;
   return true;
 }
