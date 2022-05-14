void WebFrameLoaderClient::dispatchWillPerformClientRedirect(
    const KURL& url,
    double interval,
     double fire_date) {
  expected_client_redirect_src_ = webframe_->url();
  expected_client_redirect_dest_ = webkit_glue::KURLToGURL(url);

  // TODO(timsteele): bug 1135512. Webkit does not properly notify us of
  // cancelling http > file client redirects. Since the FrameLoader's policy
  // is to never carry out such a navigation anyway, the best thing we can do
  // for now to not get confused is ignore this notification.
  if (expected_client_redirect_dest_.SchemeIsFile() &&
     (expected_client_redirect_src_.SchemeIs("http") ||
      expected_client_redirect_src_.SchemeIsSecure())) {
    expected_client_redirect_src_ = GURL();
    expected_client_redirect_dest_ = GURL();
    return;
  }
 
  if (webframe_->client()) {
     webframe_->client()->willPerformClientRedirect(
         webframe_,
         expected_client_redirect_src_,
        expected_client_redirect_dest_,
        static_cast<unsigned int>(interval),
        static_cast<unsigned int>(fire_date));
  }
}
