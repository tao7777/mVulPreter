bool CrossesExtensionProcessBoundary(
    const ExtensionSet& extensions,
    const GURL& old_url,
    const GURL& new_url,
    bool should_consider_workaround) {
  const extensions::Extension* old_url_extension = GetNonBookmarkAppExtension(
      extensions,
      old_url);
  const extensions::Extension* new_url_extension = GetNonBookmarkAppExtension(
      extensions,
      new_url);

  if (should_consider_workaround) {
    bool old_url_is_hosted_app = old_url_extension &&
        !old_url_extension->web_extent().is_empty() &&
        !AppIsolationInfo::HasIsolatedStorage(old_url_extension);
    bool new_url_is_normal_or_hosted = !new_url_extension ||
        (!new_url_extension->web_extent().is_empty() &&
         !AppIsolationInfo::HasIsolatedStorage(new_url_extension));
    bool either_is_web_store =
        (old_url_extension &&
        old_url_extension->id() == extensions::kWebStoreAppId) ||
        (new_url_extension &&
        new_url_extension->id() == extensions::kWebStoreAppId);
    if (old_url_is_hosted_app &&
        new_url_is_normal_or_hosted &&
        !either_is_web_store)
       return false;
   }
 
   return old_url_extension != new_url_extension;
 }
