void WebContentsImpl::DetachInterstitialPage() {
  if (node_.OuterContentsFrameTreeNode()) {
    if (GetRenderManager()->GetProxyToOuterDelegate()) {
      DCHECK(static_cast<RenderWidgetHostViewBase*>(
                 GetRenderManager()->current_frame_host()->GetView())
                 ->IsRenderWidgetHostViewChildFrame());
      RenderWidgetHostViewChildFrame* view =
          static_cast<RenderWidgetHostViewChildFrame*>(
              GetRenderManager()->current_frame_host()->GetView());
      GetRenderManager()->SetRWHViewForInnerContents(view);
    }
   }
 
   bool interstitial_pausing_throbber =
      ShowingInterstitialPage() && interstitial_page_->pause_throbber();
   if (ShowingInterstitialPage())
    interstitial_page_ = nullptr;
   for (auto& observer : observers_)
     observer.DidDetachInterstitialPage();
 
  if (interstitial_pausing_throbber && frame_tree_.IsLoading())
    LoadingStateChanged(true, true, nullptr);
}
