 bool ChromeContentBrowserClient::ShouldSwapProcessesForNavigation(
     const GURL& current_url,
     const GURL& new_url) {
   if (current_url.is_empty()) {
    if (new_url.SchemeIs(extensions::kExtensionScheme))
      return true;

    return false;
  }

  if (current_url.SchemeIs(extensions::kExtensionScheme) ||
      new_url.SchemeIs(extensions::kExtensionScheme)) {
    if (current_url.GetOrigin() != new_url.GetOrigin())
       return true;
   }
 
   return false;
 }
