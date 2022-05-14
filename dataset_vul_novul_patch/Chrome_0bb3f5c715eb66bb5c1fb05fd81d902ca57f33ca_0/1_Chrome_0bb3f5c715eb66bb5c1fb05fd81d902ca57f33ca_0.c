 bool RenderFrameHostManager::CanSubframeSwapProcess(
     const GURL& dest_url,
     SiteInstance* source_instance,
    SiteInstance* dest_instance,
    bool was_server_redirect) {
  DCHECK(!source_instance || !dest_instance);

  GURL resolved_url = dest_url;
  if (url::Origin::Create(resolved_url).unique()) {
    if (source_instance) {
      resolved_url = source_instance->GetSiteURL();
    } else if (dest_instance) {
       resolved_url = dest_instance->GetSiteURL();
     } else {
      if (!was_server_redirect)
         return false;
     }
   }
 
  if (!IsRendererTransferNeededForNavigation(render_frame_host_.get(),
                                             resolved_url)) {
    DCHECK(!dest_instance ||
           dest_instance == render_frame_host_->GetSiteInstance());
    return false;
  }

  return true;
}
