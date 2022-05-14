void WebFrameLoaderClient::dispatchWillPerformClientRedirect(
    const KURL& url,
    double interval,
     double fire_date) {
  if (webframe_->client()) {
    expected_client_redirect_src_ = webframe_->url();
    expected_client_redirect_dest_ = webkit_glue::KURLToGURL(url);
    if (expected_client_redirect_dest_.SchemeIsFile() &&
       (expected_client_redirect_src_.SchemeIs("http") ||
        expected_client_redirect_src_.SchemeIsSecure())) {
      expected_client_redirect_src_ = GURL();
      expected_client_redirect_dest_ = GURL();
      return;
    }
 
     webframe_->client()->willPerformClientRedirect(
         webframe_,
         expected_client_redirect_src_,
        expected_client_redirect_dest_,
        static_cast<unsigned int>(interval),
        static_cast<unsigned int>(fire_date));
  }
}
