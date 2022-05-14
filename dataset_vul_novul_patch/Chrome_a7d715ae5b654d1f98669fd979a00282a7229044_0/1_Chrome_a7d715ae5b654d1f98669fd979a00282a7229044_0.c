bool LocalFrameClientImpl::NavigateBackForward(int offset) const {
  WebViewImpl* webview = web_frame_->ViewImpl();
  if (!webview->Client())
    return false;

  DCHECK(offset);
  if (offset > webview->Client()->HistoryForwardListCount())
     return false;
   if (offset < -webview->Client()->HistoryBackListCount())
     return false;
  webview->Client()->NavigateBackForwardSoon(offset);
   return true;
 }
