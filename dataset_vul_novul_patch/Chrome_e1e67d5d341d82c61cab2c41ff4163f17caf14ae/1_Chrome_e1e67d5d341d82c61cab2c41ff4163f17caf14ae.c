UserInitiatedInfo CreateUserInitiatedInfo(
    content::NavigationHandle* navigation_handle,
    PageLoadTracker* committed_load) {
  if (!navigation_handle->IsRendererInitiated())
     return UserInitiatedInfo::BrowserInitiated();
 
   return UserInitiatedInfo::RenderInitiated(
      navigation_handle->HasUserGesture());
 }
