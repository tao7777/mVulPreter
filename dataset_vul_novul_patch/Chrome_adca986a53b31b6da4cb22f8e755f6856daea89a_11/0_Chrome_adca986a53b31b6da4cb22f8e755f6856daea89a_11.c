 bool WebContentsImpl::IsLoading() const {
   return frame_tree_.IsLoading() &&
         !(ShowingInterstitialPage() && interstitial_page_->pause_throbber());
 }
