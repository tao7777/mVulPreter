void WebContentsImpl::DidFailProvisionalLoadWithError(
    RenderViewHost* render_view_host,
    const ViewHostMsg_DidFailProvisionalLoadWithError_Params& params) {
  VLOG(1) << "Failed Provisional Load: " << params.url.possibly_invalid_spec()
          << ", error_code: " << params.error_code
          << ", error_description: " << params.error_description
          << ", is_main_frame: " << params.is_main_frame
          << ", showing_repost_interstitial: " <<
            params.showing_repost_interstitial
          << ", frame_id: " << params.frame_id;
  GURL validated_url(params.url);
  RenderProcessHost* render_process_host =
      render_view_host->GetProcess();
  RenderViewHost::FilterURL(render_process_host, false, &validated_url);

  if (net::ERR_ABORTED == params.error_code) {
    if (ShowingInterstitialPage()) {
      LOG(WARNING) << "Discarding message during interstitial.";
       return;
     }
 
     render_manager_.RendererAbortedProvisionalLoad(render_view_host);
   }
 
  // Do not usually clear the pending entry if one exists, so that the user's
  // typed URL is not lost when a navigation fails or is aborted.  However, in
  // cases that we don't show the pending entry (e.g., renderer-initiated
  // navigations in an existing tab), we don't keep it around.  That prevents
  // spoofs on in-page navigations that don't go through
  // DidStartProvisionalLoadForFrame.
  // In general, we allow the view to clear the pending entry and typed URL if
  // the user requests (e.g., hitting Escape with focus in the address bar).
  // Note: don't touch the transient entry, since an interstitial may exist.
  if (controller_.GetPendingEntry() != controller_.GetVisibleEntry())
    controller_.DiscardPendingEntry();

   FOR_EACH_OBSERVER(WebContentsObserver,
                     observers_,
                     DidFailProvisionalLoad(params.frame_id,
                                           params.is_main_frame,
                                           validated_url,
                                           params.error_code,
                                           params.error_description,
                                           render_view_host));
}
