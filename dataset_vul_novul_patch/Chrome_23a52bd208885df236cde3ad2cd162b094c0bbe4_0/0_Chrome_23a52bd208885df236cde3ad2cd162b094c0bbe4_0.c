 bool ExtensionResourceRequestPolicy::CanRequestResource(
     const GURL& resource_url,
    const WebKit::WebFrame* frame,
     const ExtensionSet* loaded_extensions) {
   CHECK(resource_url.SchemeIs(chrome::kExtensionScheme));
 
  const Extension* extension =
      loaded_extensions->GetExtensionOrAppByURL(ExtensionURLInfo(resource_url));
  if (!extension) {
    return true;
  }

  std::string resource_root_relative_path =
      resource_url.path().empty() ? "" : resource_url.path().substr(1);
  if (extension->is_hosted_app() &&
      !extension->icons().ContainsPath(resource_root_relative_path)) {
    LOG(ERROR) << "Denying load of " << resource_url.spec() << " from "
               << "hosted app.";
    return false;
  }

 
  GURL frame_url = frame->document().url();
  GURL page_url = frame->top()->document().url();
  // - devtools (chrome-extension:// URLs are loaded into frames of devtools
  //     to support the devtools extension APIs)
   if (!CommandLine::ForCurrentProcess()->HasSwitch(
           switches::kDisableExtensionsResourceWhitelist) &&
       !frame_url.is_empty() &&
       !frame_url.SchemeIs(chrome::kExtensionScheme) &&
      !(page_url.SchemeIs(chrome::kChromeDevToolsScheme) &&
          !extension->devtools_url().is_empty()) &&
       !extension->IsResourceWebAccessible(resource_url.path())) {
     LOG(ERROR) << "Denying load of " << resource_url.spec() << " which "
                << "is not a web accessible resource.";
    return false;
  }

  return true;
}
