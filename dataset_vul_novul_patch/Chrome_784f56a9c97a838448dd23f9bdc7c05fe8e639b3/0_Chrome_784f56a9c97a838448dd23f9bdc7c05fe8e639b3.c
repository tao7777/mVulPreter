void NavigatorImpl::DidNavigate(
    RenderFrameHostImpl* render_frame_host,
    const FrameHostMsg_DidCommitProvisionalLoad_Params& params,
    std::unique_ptr<NavigationHandleImpl> navigation_handle) {
  FrameTree* frame_tree = render_frame_host->frame_tree_node()->frame_tree();
  bool oopifs_possible = SiteIsolationPolicy::AreCrossProcessFramesPossible();

  bool is_navigation_within_page = controller_->IsURLInPageNavigation(
      params.url, params.origin, params.was_within_same_document,
      render_frame_host);

  if (is_navigation_within_page &&
      render_frame_host !=
          render_frame_host->frame_tree_node()
              ->render_manager()
              ->current_frame_host()) {
    bad_message::ReceivedBadMessage(render_frame_host->GetProcess(),
                                    bad_message::NI_IN_PAGE_NAVIGATION);
    is_navigation_within_page = false;
  }

  if (ui::PageTransitionIsMainFrame(params.transition)) {
    if (delegate_) {
      if (delegate_->CanOverscrollContent()) {
        if (!params.was_within_same_document)
          controller_->TakeScreenshot();
      }

      delegate_->DidNavigateMainFramePreCommit(is_navigation_within_page);
    }

    if (!oopifs_possible)
      frame_tree->root()->render_manager()->DidNavigateFrame(
          render_frame_host, params.gesture == NavigationGestureUser);
  }

  render_frame_host->frame_tree_node()->SetCurrentOrigin(
      params.origin, params.has_potentially_trustworthy_unique_origin);

  render_frame_host->frame_tree_node()->SetInsecureRequestPolicy(
      params.insecure_request_policy);

  if (!is_navigation_within_page) {
    render_frame_host->ResetContentSecurityPolicies();
    render_frame_host->frame_tree_node()->ResetCspHeaders();
    render_frame_host->frame_tree_node()->ResetFeaturePolicyHeader();
  }

  if (oopifs_possible) {
    FrameTreeNode* frame = render_frame_host->frame_tree_node();
    frame->render_manager()->DidNavigateFrame(
        render_frame_host, params.gesture == NavigationGestureUser);
  }

  SiteInstanceImpl* site_instance = render_frame_host->GetSiteInstance();
  if (!site_instance->HasSite() && ShouldAssignSiteForURL(params.url) &&
      !params.url_is_unreachable) {
    site_instance->SetSite(params.url);
  }

  if (ui::PageTransitionIsMainFrame(params.transition) && delegate_)
    delegate_->SetMainFrameMimeType(params.contents_mime_type);

  int old_entry_count = controller_->GetEntryCount();
  LoadCommittedDetails details;
  bool did_navigate = controller_->RendererDidNavigate(
      render_frame_host, params, &details, is_navigation_within_page,
      navigation_handle.get());

  if (old_entry_count != controller_->GetEntryCount() ||
      details.previous_entry_index !=
          controller_->GetLastCommittedEntryIndex()) {
    frame_tree->root()->render_manager()->SendPageMessage(
        new PageMsg_SetHistoryOffsetAndLength(
            MSG_ROUTING_NONE, controller_->GetLastCommittedEntryIndex(),
            controller_->GetEntryCount()),
        site_instance);
  }

  render_frame_host->frame_tree_node()->SetCurrentURL(params.url);
  render_frame_host->SetLastCommittedOrigin(params.origin);

  if (!params.url_is_unreachable)
    render_frame_host->set_last_successful_url(params.url);
 
  if (!is_navigation_within_page)
     render_frame_host->ResetFeaturePolicy();
 
  if (details.type != NAVIGATION_TYPE_NAV_IGNORE && delegate_) {
    DCHECK_EQ(!render_frame_host->GetParent(),
              did_navigate ? details.is_main_frame : false);
    navigation_handle->DidCommitNavigation(params, did_navigate,
                                           details.did_replace_entry,
                                           details.previous_url, details.type,
                                           render_frame_host);
    navigation_handle.reset();
  }

  if (!did_navigate)
    return;  // No navigation happened.


  RecordNavigationMetrics(details, params, site_instance);

  if (delegate_) {
    if (details.is_main_frame) {
      delegate_->DidNavigateMainFramePostCommit(render_frame_host,
                                                details, params);
    }

    delegate_->DidNavigateAnyFramePostCommit(
        render_frame_host, details, params);
  }
}
