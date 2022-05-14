void WebContentsImpl::AttachInterstitialPage(
     InterstitialPageImpl* interstitial_page) {
   DCHECK(interstitial_page);
   render_manager_.set_interstitial_page(interstitial_page);
   FOR_EACH_OBSERVER(WebContentsObserver, observers_,
                     DidAttachInterstitialPage());
 }
