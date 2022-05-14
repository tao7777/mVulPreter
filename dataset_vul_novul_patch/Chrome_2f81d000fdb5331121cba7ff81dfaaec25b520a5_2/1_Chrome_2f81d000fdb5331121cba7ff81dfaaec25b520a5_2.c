bool DownloadManagerImpl::InterceptDownload(
    const download::DownloadCreateInfo& info) {
  WebContents* web_contents = WebContentsImpl::FromRenderFrameHostID(
      info.render_process_id, info.render_frame_id);
  if (info.is_new_download &&
      info.result ==
          download::DOWNLOAD_INTERRUPT_REASON_SERVER_CROSS_ORIGIN_REDIRECT) {
    if (web_contents) {
      std::vector<GURL> url_chain(info.url_chain);
      GURL url = url_chain.back();
      url_chain.pop_back();
      NavigationController::LoadURLParams params(url);
      params.has_user_gesture = info.has_user_gesture;
      params.referrer = Referrer(
           info.referrer_url, Referrer::NetReferrerPolicyToBlinkReferrerPolicy(
                                  info.referrer_policy));
       params.redirect_chain = url_chain;
       web_contents->GetController().LoadURLWithParams(params);
     }
     if (info.request_handle)
      info.request_handle->CancelRequest(false);
    return true;
  }
  if (!delegate_ ||
      !delegate_->InterceptDownloadIfApplicable(
          info.url(), info.mime_type, info.request_origin, web_contents)) {
    return false;
  }
  if (info.request_handle)
    info.request_handle->CancelRequest(false);
  return true;
}
