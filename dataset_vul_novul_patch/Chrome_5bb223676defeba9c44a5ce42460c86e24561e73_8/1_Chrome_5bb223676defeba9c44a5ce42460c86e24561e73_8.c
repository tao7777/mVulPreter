ExtensionsGuestViewMessageFilter::MaybeCreateThrottle(
    NavigationHandle* handle) {
  DCHECK(content::MimeHandlerViewMode::UsesCrossProcessFrame());
  if (!handle->GetParentFrame()) {
    return nullptr;
  }
  int32_t parent_process_id = handle->GetParentFrame()->GetProcess()->GetID();
  auto& map = *GetProcessIdToFilterMap();
  if (!base::ContainsKey(map, parent_process_id) || !map[parent_process_id]) {
    return nullptr;
  }
  for (auto& pair : map[parent_process_id]->frame_navigation_helpers_) {
    if (!pair.second->ShouldCancelAndIgnore(handle))
      continue;
    return std::make_unique<CancelAndIgnoreNavigationForPluginFrameThrottle>(
        handle);
  }
  return nullptr;
}
