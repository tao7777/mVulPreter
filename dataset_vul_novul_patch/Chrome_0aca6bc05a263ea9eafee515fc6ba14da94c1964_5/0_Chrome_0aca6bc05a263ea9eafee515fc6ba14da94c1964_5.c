bool PermissionsData::CanCaptureVisiblePage(int tab_id,
bool PermissionsData::CanCaptureVisiblePage(const GURL& document_url,
                                            const Extension* extension,
                                            int tab_id,
                                             std::string* error) const {
  bool has_active_tab = false;
  {
    base::AutoLock auto_lock(runtime_lock_);
    const PermissionSet* tab_permissions = GetTabSpecificPermissions(tab_id);
    has_active_tab = tab_permissions &&
                     tab_permissions->HasAPIPermission(APIPermission::kTab);
  }
  // We check GetPageAccess() (in addition the the <all_urls> and activeTab
  // checks below) for the case of URLs that can be conditionally granted (such
  // as file:// URLs or chrome:// URLs for component extensions), and to respect
  // policy restrictions, if any. If an extension has <all_urls>,
  // GetPageAccess() will still (correctly) return false if, for instance, the
  // URL is a file:// URL and the extension does not have file access.
  // See https://crbug.com/810220.
  if (GetPageAccess(extension, document_url, tab_id, error) != ACCESS_ALLOWED) {
    if (!document_url.SchemeIs(content::kChromeUIScheme))
      return false;

    // Most extensions will not have (and cannot get) access to chrome:// URLs
    // (which are restricted). However, allowing them to capture these URLs can
    // be useful, such as in the case of capturing a screenshot. Allow
    // extensions that have been explicitly invoked (and have the activeTab)
    // permission to capture chrome:// URLs.
    if (has_active_tab)
      return true;

    if (error)
      *error = manifest_errors::kActiveTabPermissionNotGranted;

    return false;
  }

   const URLPattern all_urls(URLPattern::SCHEME_ALL,
                             URLPattern::kAllUrlsPattern);
 
   base::AutoLock auto_lock(runtime_lock_);
   if (active_permissions_unsafe_->explicit_hosts().ContainsPattern(all_urls))
     return true;
 
  const PermissionSet* tab_permissions = GetTabSpecificPermissions(tab_id);
  if (tab_permissions &&
      tab_permissions->HasAPIPermission(APIPermission::kTab)) {
    return true;
   }
 
   if (error)
    *error = manifest_errors::kAllURLOrActiveTabNeeded;
  return false;
}
