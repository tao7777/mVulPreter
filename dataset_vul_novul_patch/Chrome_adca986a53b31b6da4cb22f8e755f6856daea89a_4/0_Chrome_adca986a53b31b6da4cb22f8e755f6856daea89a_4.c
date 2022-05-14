 void WebContentsImpl::AttachInterstitialPage(
     InterstitialPageImpl* interstitial_page) {
  DCHECK(!interstitial_page_ && interstitial_page);
  interstitial_page_ = interstitial_page;
 
  CancelActiveAndPendingDialogs();

  for (auto& observer : observers_)
    observer.DidAttachInterstitialPage();

  if (frame_tree_.IsLoading())
    LoadingStateChanged(true, true, nullptr);

  if (node_.OuterContentsFrameTreeNode()) {
    if (GetRenderManager()->GetProxyToOuterDelegate()) {
      DCHECK(
          static_cast<RenderWidgetHostViewBase*>(interstitial_page->GetView())
              ->IsRenderWidgetHostViewChildFrame());
      RenderWidgetHostViewChildFrame* view =
          static_cast<RenderWidgetHostViewChildFrame*>(
              interstitial_page->GetView());
      GetRenderManager()->SetRWHViewForInnerContents(view);
    }
  }
}
