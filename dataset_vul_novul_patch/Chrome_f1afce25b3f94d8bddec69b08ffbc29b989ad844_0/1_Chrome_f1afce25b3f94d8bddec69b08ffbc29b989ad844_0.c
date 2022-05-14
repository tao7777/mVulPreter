bool ChromeContentBrowserClientExtensionsPart::ShouldAllowOpenURL(
    content::SiteInstance* site_instance,
    const GURL& to_url,
    bool* result) {
  DCHECK(result);

  url::Origin to_origin(to_url);
  if (to_origin.scheme() != kExtensionScheme) {
    return false;
  }


  ExtensionRegistry* registry =
      ExtensionRegistry::Get(site_instance->GetBrowserContext());
   const Extension* to_extension =
       registry->enabled_extensions().GetByID(to_origin.host());
   if (!to_extension) {
    *result = true;
     return true;
   }
 
  GURL site_url(site_instance->GetSiteURL());
  const Extension* from_extension =
      registry->enabled_extensions().GetExtensionOrAppByURL(site_url);
  if (from_extension && from_extension == to_extension) {
    *result = true;
    return true;
  }

  if (to_url.SchemeIsFileSystem() || to_url.SchemeIsBlob()) {
    if (to_url.SchemeIsFileSystem())
      RecordShouldAllowOpenURLFailure(FAILURE_FILE_SYSTEM_URL, site_url);
    else
      RecordShouldAllowOpenURLFailure(FAILURE_BLOB_URL, site_url);

    char site_url_copy[256];
    base::strlcpy(site_url_copy, site_url.spec().c_str(),
                  arraysize(site_url_copy));
    base::debug::Alias(&site_url_copy);
    char to_origin_copy[256];
    base::strlcpy(to_origin_copy, to_origin.Serialize().c_str(),
                  arraysize(to_origin_copy));
    base::debug::Alias(&to_origin_copy);
    base::debug::DumpWithoutCrashing();

    *result = false;
    return true;
  }

  if (site_url.SchemeIs(content::kChromeUIScheme) ||
      site_url.SchemeIs(content::kChromeDevToolsScheme) ||
      site_url.SchemeIs(chrome::kChromeSearchScheme)) {
    *result = true;
    return true;
  }

  if (site_url.SchemeIs(content::kGuestScheme)) {
    *result = true;
    return true;
  }

  if (WebAccessibleResourcesInfo::IsResourceWebAccessible(to_extension,
                                                          to_url.path())) {
    *result = true;
    return true;
  }

  if (!site_url.SchemeIsHTTPOrHTTPS() && !site_url.SchemeIs(kExtensionScheme)) {
    RecordShouldAllowOpenURLFailure(
        FAILURE_SCHEME_NOT_HTTP_OR_HTTPS_OR_EXTENSION, site_url);
  } else {
    RecordShouldAllowOpenURLFailure(FAILURE_RESOURCE_NOT_WEB_ACCESSIBLE,
                                    site_url);
  }

  *result = false;
  return true;
}
