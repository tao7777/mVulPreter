bool LocalFrameClientImpl::NavigateBackForward(int offset) const {
  WebViewImpl* webview = web_frame_->ViewImpl();
  if (!webview->Client())
    return false;

  DCHECK(offset);
  if (offset > webview->Client()->HistoryForwardListCount())
     return false;
   if (offset < -webview->Client()->HistoryBackListCount())
     return false;

  bool has_user_gesture =
      Frame::HasTransientUserActivation(web_frame_->GetFrame());
  webview->Client()->NavigateBackForwardSoon(offset, has_user_gesture);
   return true;
 }
