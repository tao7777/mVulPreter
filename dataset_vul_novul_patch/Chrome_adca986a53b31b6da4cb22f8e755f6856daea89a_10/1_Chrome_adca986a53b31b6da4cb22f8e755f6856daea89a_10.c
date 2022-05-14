RenderWidgetHostImpl* WebContentsImpl::GetRenderWidgetHostWithPageFocus() {
  WebContentsImpl* focused_web_contents = GetFocusedWebContents();
 
   if (focused_web_contents->ShowingInterstitialPage()) {
     return static_cast<RenderFrameHostImpl*>(
               focused_web_contents->GetRenderManager()
                   ->interstitial_page()
                   ->GetMainFrame())
         ->GetRenderWidgetHost();
   }
 
  return focused_web_contents->GetMainFrame()->GetRenderWidgetHost();
}
