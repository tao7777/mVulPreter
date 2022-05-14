bool ChromeContentRendererClient::ShouldFork(WebFrame* frame,
                                             const GURL& url,
                                             bool is_initial_navigation,
                                             bool* send_referrer) {
  DCHECK(!frame->parent());

  if (prerender_dispatcher_.get() && prerender_dispatcher_->IsPrerenderURL(url))
    return true;

  const ExtensionSet* extensions = extension_dispatcher_->extensions();

  const Extension* new_url_extension = extensions::GetNonBookmarkAppExtension(
      *extensions, ExtensionURLInfo(url));
  bool is_extension_url = !!new_url_extension;

  if (CrossesExtensionExtents(frame, url, *extensions, is_extension_url,
          is_initial_navigation)) {
    *send_referrer = true;

    const Extension* extension =
        extension_dispatcher_->extensions()->GetExtensionOrAppByURL(
            ExtensionURLInfo(url));
    if (extension && extension->is_app()) {
      UMA_HISTOGRAM_ENUMERATION(
          extension_misc::kAppLaunchHistogram,
          extension_misc::APP_LAUNCH_CONTENT_NAVIGATION,
          extension_misc::APP_LAUNCH_BUCKET_BOUNDARY);
    }
    return true;
  }

  if (frame->top()->document().url() == url) {
    if (is_extension_url != extension_dispatcher_->is_extension_process())
       return true;
   }
 
   return false;
 }
