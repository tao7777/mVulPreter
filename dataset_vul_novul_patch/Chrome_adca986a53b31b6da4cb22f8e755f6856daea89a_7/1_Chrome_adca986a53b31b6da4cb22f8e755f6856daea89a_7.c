 void WebContentsImpl::FocusThroughTabTraversal(bool reverse) {
   if (ShowingInterstitialPage()) {
    GetRenderManager()->interstitial_page()->FocusThroughTabTraversal(reverse);
     return;
   }
   RenderWidgetHostView* const fullscreen_view =
      GetFullscreenRenderWidgetHostView();
  if (fullscreen_view) {
    fullscreen_view->Focus();
    return;
  }
  GetRenderViewHost()->SetInitialFocus(reverse);
 }
