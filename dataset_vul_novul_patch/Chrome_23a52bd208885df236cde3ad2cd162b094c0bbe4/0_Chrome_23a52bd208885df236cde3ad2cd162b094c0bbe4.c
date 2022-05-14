bool ChromeContentRendererClient::WillSendRequest(WebKit::WebFrame* frame,
                                                  const GURL& url,
                                                  GURL* new_url) {
   if (url.SchemeIs(chrome::kExtensionScheme) &&
       !ExtensionResourceRequestPolicy::CanRequestResource(
           url,
          frame,
           extension_dispatcher_->extensions())) {
     *new_url = GURL("chrome-extension://invalid/");
     return true;
  }

  return false;
}
