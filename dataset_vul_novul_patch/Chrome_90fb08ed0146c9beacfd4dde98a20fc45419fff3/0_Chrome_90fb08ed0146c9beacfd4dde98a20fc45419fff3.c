void WebContentsImpl::AttachInterstitialPage(
     InterstitialPageImpl* interstitial_page) {
   DCHECK(interstitial_page);
   render_manager_.set_interstitial_page(interstitial_page);

  // Cancel any visible dialogs so that they don't interfere with the
  // interstitial.
  if (dialog_manager_)
    dialog_manager_->CancelActiveAndPendingDialogs(this);

   FOR_EACH_OBSERVER(WebContentsObserver, observers_,
                     DidAttachInterstitialPage());
 }
