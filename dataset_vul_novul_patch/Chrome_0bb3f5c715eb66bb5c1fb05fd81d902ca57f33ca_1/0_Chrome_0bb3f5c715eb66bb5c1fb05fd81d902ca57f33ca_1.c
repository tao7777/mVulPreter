RenderFrameHostManager::GetSiteInstanceForNavigationRequest(
    const NavigationRequest& request) {
  SiteInstance* current_site_instance = render_frame_host_->GetSiteInstance();
  bool no_renderer_swap_allowed = false;
  bool was_server_redirect = request.navigation_handle() &&
                             request.navigation_handle()->WasServerRedirect();

  if (frame_tree_node_->IsMainFrame()) {
    bool can_renderer_initiate_transfer =
        (request.state() == NavigationRequest::FAILED &&
         SiteIsolationPolicy::IsErrorPageIsolationEnabled(
             true /* in_main_frame */)) ||
        (render_frame_host_->IsRenderFrameLive() &&
         IsURLHandledByNetworkStack(request.common_params().url) &&
         IsRendererTransferNeededForNavigation(render_frame_host_.get(),
                                               request.common_params().url));
    no_renderer_swap_allowed |=
        request.from_begin_navigation() && !can_renderer_initiate_transfer;
  } else {
     no_renderer_swap_allowed |= !CanSubframeSwapProcess(
         request.common_params().url, request.source_site_instance(),
        request.dest_site_instance());
   }
 
   if (no_renderer_swap_allowed)
    return scoped_refptr<SiteInstance>(current_site_instance);

  SiteInstance* candidate_site_instance =
      speculative_render_frame_host_
          ? speculative_render_frame_host_->GetSiteInstance()
          : nullptr;

  scoped_refptr<SiteInstance> dest_site_instance = GetSiteInstanceForNavigation(
      request.common_params().url, request.source_site_instance(),
      request.dest_site_instance(), candidate_site_instance,
      request.common_params().transition,
      request.state() == NavigationRequest::FAILED,
      request.restore_type() != RestoreType::NONE, request.is_view_source(),
      was_server_redirect);

  return dest_site_instance;
}
