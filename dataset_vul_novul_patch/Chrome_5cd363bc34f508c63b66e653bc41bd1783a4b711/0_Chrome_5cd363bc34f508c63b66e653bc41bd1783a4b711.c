RenderFrameHostImpl* RenderFrameHostManager::GetFrameHostForNavigation(
    const NavigationRequest& request) {
  DCHECK(!request.common_params().url.SchemeIs(url::kJavaScriptScheme))
      << "Don't call this method for JavaScript URLs as those create a "
         "temporary  NavigationRequest and we don't want to reset an ongoing "
         "navigation's speculative RFH.";

  RenderFrameHostImpl* navigation_rfh = nullptr;

  SiteInstance* current_site_instance = render_frame_host_->GetSiteInstance();
  scoped_refptr<SiteInstance> dest_site_instance =
      GetSiteInstanceForNavigationRequest(request);

  bool use_current_rfh = current_site_instance == dest_site_instance;

  bool notify_webui_of_rf_creation = false;
  if (use_current_rfh) {
     if (speculative_render_frame_host_) {
      // NavigationEntry stopped if needed. This is the case if the new
      // navigation was started from BeginNavigation. If the navigation was
      // started through the NavigationController, the NavigationController has
      // already updated its state properly, and doesn't need to be notified.
      if (speculative_render_frame_host_->navigation_handle() &&
          request.from_begin_navigation()) {
         frame_tree_node_->navigator()->DiscardPendingEntryIfNeeded(
             speculative_render_frame_host_->navigation_handle()
                 ->pending_nav_entry_id());
      }
      DiscardUnusedFrame(UnsetSpeculativeRenderFrameHost());
    }

    if (frame_tree_node_->IsMainFrame()) {
      UpdatePendingWebUIOnCurrentFrameHost(request.common_params().url,
                                           request.bindings());
    }

    navigation_rfh = render_frame_host_.get();

    DCHECK(!speculative_render_frame_host_);
  } else {

    if (!speculative_render_frame_host_ ||
        speculative_render_frame_host_->GetSiteInstance() !=
            dest_site_instance.get()) {
      CleanUpNavigation();
      bool success = CreateSpeculativeRenderFrameHost(current_site_instance,
                                                      dest_site_instance.get());
      DCHECK(success);
    }
    DCHECK(speculative_render_frame_host_);

    if (frame_tree_node_->IsMainFrame()) {
      bool changed_web_ui = speculative_render_frame_host_->UpdatePendingWebUI(
          request.common_params().url, request.bindings());
      speculative_render_frame_host_->CommitPendingWebUI();
      DCHECK_EQ(GetNavigatingWebUI(), speculative_render_frame_host_->web_ui());
      notify_webui_of_rf_creation =
          changed_web_ui && speculative_render_frame_host_->web_ui();
    }
    navigation_rfh = speculative_render_frame_host_.get();

    if (!render_frame_host_->IsRenderFrameLive()) {
      if (GetRenderFrameProxyHost(dest_site_instance.get())) {
        navigation_rfh->Send(
            new FrameMsg_SwapIn(navigation_rfh->GetRoutingID()));
      }
      CommitPending();

      if (notify_webui_of_rf_creation && render_frame_host_->web_ui()) {
        render_frame_host_->web_ui()->RenderFrameCreated(
            render_frame_host_.get());
        notify_webui_of_rf_creation = false;
      }
    }
  }
  DCHECK(navigation_rfh &&
         (navigation_rfh == render_frame_host_.get() ||
          navigation_rfh == speculative_render_frame_host_.get()));

  if (!navigation_rfh->IsRenderFrameLive()) {
    if (!ReinitializeRenderFrame(navigation_rfh))
      return nullptr;

    notify_webui_of_rf_creation = true;

    if (navigation_rfh == render_frame_host_.get()) {
      EnsureRenderFrameHostVisibilityConsistent();
      EnsureRenderFrameHostPageFocusConsistent();
      delegate_->NotifyMainFrameSwappedFromRenderManager(
          nullptr, render_frame_host_->render_view_host());
    }
  }

  if (notify_webui_of_rf_creation && GetNavigatingWebUI() &&
      frame_tree_node_->IsMainFrame()) {
    GetNavigatingWebUI()->RenderFrameCreated(navigation_rfh);
  }

  return navigation_rfh;
}
