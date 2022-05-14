 bool RenderFrameHostManager::CanSubframeSwapProcess(
     const GURL& dest_url,
     SiteInstance* source_instance,
    SiteInstance* dest_instance) {
  DCHECK(!source_instance || !dest_instance);

  GURL resolved_url = dest_url;
  if (url::Origin::Create(resolved_url).unique()) {
    if (source_instance) {
      resolved_url = source_instance->GetSiteURL();
    } else if (dest_instance) {
       resolved_url = dest_instance->GetSiteURL();
     } else {
      // then check whether it is safe to put into the parent frame's process.
      // This is the case for about:blank URLs (with or without fragments),
      // since they contain no active data.  This is also the case for
      // about:srcdoc, since such URLs only get active content from their parent
      // frame.  Using the parent frame's process avoids putting blank frames
      // into OOPIFs and preserves scripting for about:srcdoc.
      //
      // Allow a process swap for other unique origin URLs, such as data: URLs.
      // These have active content and may have come from an untrusted source,
      // such as a restored frame from a different site or a redirect.
      // (Normally, redirects to data: or about: URLs are disallowed as
      // example, see ExtensionWebRequestApiTest.WebRequestDeclarative1).)
      if (resolved_url.IsAboutBlank() ||
          resolved_url == GURL(content::kAboutSrcDocURL)) {
         return false;
      }
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
