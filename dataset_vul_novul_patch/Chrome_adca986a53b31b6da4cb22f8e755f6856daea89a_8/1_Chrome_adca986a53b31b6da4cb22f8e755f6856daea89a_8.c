RenderWidgetHostImpl* WebContentsImpl::GetFocusedRenderWidgetHost(
    RenderWidgetHostImpl* receiving_widget) {
  if (!SiteIsolationPolicy::AreCrossProcessFramesPossible())
    return receiving_widget;

  if (receiving_widget != GetMainFrame()->GetRenderWidgetHost())
    return receiving_widget;

  WebContentsImpl* focused_contents = GetFocusedWebContents();

   if (focused_contents->ShowingInterstitialPage()) {
     return static_cast<RenderFrameHostImpl*>(
               focused_contents->GetRenderManager()
                   ->interstitial_page()
                   ->GetMainFrame())
         ->GetRenderWidgetHost();
   }
 
  FrameTreeNode* focused_frame = nullptr;
  if (focused_contents->browser_plugin_guest_ &&
      !GuestMode::IsCrossProcessFrameGuest(focused_contents)) {
    focused_frame = frame_tree_.GetFocusedFrame();
  } else {
    focused_frame = GetFocusedWebContents()->frame_tree_.GetFocusedFrame();
  }

  if (!focused_frame)
    return receiving_widget;

  RenderWidgetHostView* view = focused_frame->current_frame_host()->GetView();
  if (!view)
    return nullptr;

  return RenderWidgetHostImpl::From(view->GetRenderWidgetHost());
}
