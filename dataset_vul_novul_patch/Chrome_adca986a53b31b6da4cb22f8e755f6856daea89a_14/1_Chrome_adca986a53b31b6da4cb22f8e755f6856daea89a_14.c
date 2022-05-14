 bool WebContentsImpl::ShowingInterstitialPage() const {
  return GetRenderManager()->interstitial_page() != NULL;
 }
