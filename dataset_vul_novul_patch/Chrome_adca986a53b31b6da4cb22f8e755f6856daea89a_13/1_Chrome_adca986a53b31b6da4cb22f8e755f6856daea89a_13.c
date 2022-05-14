void WebContentsImpl::SetAsFocusedWebContentsIfNecessary() {
  WebContentsImpl* old_contents = GetFocusedWebContents();
  if (old_contents == this)
    return;

  GetOutermostWebContents()->node_.SetFocusedWebContents(this);

  if (!GuestMode::IsCrossProcessFrameGuest(this) && browser_plugin_guest_)
    return;

  if (old_contents)
    old_contents->GetMainFrame()->GetRenderWidgetHost()->SetPageFocus(false);

  if (GetRenderManager()->GetProxyToOuterDelegate())
     GetRenderManager()->GetProxyToOuterDelegate()->SetFocusedFrame();
 
   if (ShowingInterstitialPage()) {
    static_cast<RenderFrameHostImpl*>(
        GetRenderManager()->interstitial_page()->GetMainFrame())
         ->GetRenderWidgetHost()
         ->SetPageFocus(true);
   } else {
    GetMainFrame()->GetRenderWidgetHost()->SetPageFocus(true);
  }
}
