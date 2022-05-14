 bool ChromeContentBrowserClient::ShouldSwapProcessesForNavigation(
    SiteInstance* site_instance,
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
 
  // The checks below only matter if we can retrieve which extensions are
  // installed.
  Profile* profile =
      Profile::FromBrowserContext(site_instance->GetBrowserContext());
  ExtensionService* service =
      extensions::ExtensionSystem::Get(profile)->extension_service();
  if (!service)
    return false;

  // We must swap if the URL is for an extension and we are not using an
  // extension process.
  const Extension* new_extension =
      service->extensions()->GetExtensionOrAppByURL(ExtensionURLInfo(new_url));
  // Ignore all hosted apps except the Chrome Web Store, since they do not
  // require their own BrowsingInstance (e.g., postMessage is ok).
  if (new_extension &&
      new_extension->is_hosted_app() &&
      new_extension->id() != extension_misc::kWebStoreAppId)
    new_extension = NULL;
  if (new_extension &&
      site_instance->HasProcess() &&
      !service->process_map()->Contains(new_extension->id(),
                                        site_instance->GetProcess()->GetID()))
    return true;

   return false;
 }
