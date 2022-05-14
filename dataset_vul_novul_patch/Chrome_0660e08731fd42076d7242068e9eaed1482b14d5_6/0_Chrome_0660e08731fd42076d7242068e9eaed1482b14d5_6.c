bool PermissionsData::CanCaptureVisiblePage(const GURL& document_url,
bool PermissionsData::CanCaptureVisiblePage(
    const GURL& document_url,
    int tab_id,
    std::string* error,
    CaptureRequirement capture_requirement) const {
   bool has_active_tab = false;
   bool has_all_urls = false;
  bool has_page_capture = false;
  url::Origin origin = url::Origin::Create(document_url);
  const GURL origin_url = origin.GetURL();
  {
    base::AutoLock auto_lock(runtime_lock_);
    if (location_ != Manifest::COMPONENT &&
        IsPolicyBlockedHostUnsafe(origin_url)) {
      if (error)
        *error = extension_misc::kPolicyBlockedScripting;
      return false;
    }

    const PermissionSet* tab_permissions = GetTabSpecificPermissions(tab_id);
    has_active_tab = tab_permissions &&
                     tab_permissions->HasAPIPermission(APIPermission::kTab);

    const URLPattern all_urls(URLPattern::SCHEME_ALL,
                               URLPattern::kAllUrlsPattern);
     has_all_urls =
         active_permissions_unsafe_->explicit_hosts().ContainsPattern(all_urls);
    has_page_capture = active_permissions_unsafe_->HasAPIPermission(
        APIPermission::kPageCapture);
   }
  std::string access_error;
  if (capture_requirement == CaptureRequirement::kActiveTabOrAllUrls) {
    if (!has_active_tab && !has_all_urls) {
      if (error)
        *error = manifest_errors::kAllURLOrActiveTabNeeded;
      return false;
    }
 
    // We check GetPageAccess() (in addition to the <all_urls> and activeTab
    // checks below) for the case of URLs that can be conditionally granted
    // (such as file:// URLs or chrome:// URLs for component extensions). If an
    // extension has <all_urls>, GetPageAccess() will still (correctly) return
    // false if, for instance, the URL is a file:// URL and the extension does
    // not have file access. See https://crbug.com/810220. If the extension has
    // page access (and has activeTab or <all_urls>), allow the capture.
    if (GetPageAccess(origin_url, tab_id, &access_error) ==
        PageAccess::kAllowed)
      return true;
  } else {
    DCHECK_EQ(CaptureRequirement::kPageCapture, capture_requirement);
    if (!has_page_capture) {
      if (error)
        *error = manifest_errors::kPageCaptureNeeded;
    }
 
    // If the URL is a typical web URL, the pageCapture permission is
    // sufficient.
    if ((origin_url.SchemeIs(url::kHttpScheme) ||
         origin_url.SchemeIs(url::kHttpsScheme)) &&
        !origin.IsSameOriginWith(url::Origin::Create(
            ExtensionsClient::Get()->GetWebstoreBaseURL()))) {
      return true;
    }
  }
 
 
  // activeTab (since the extension scheme is not included in the list of
  // valid schemes for extension permissions). To capture an extension's own
  // page, either activeTab or <all_urls> is needed (it's no higher privilege
  // than a normal web page). At least one of these is still needed because
  // the extension page may have embedded web content.
   if (origin_url.host() == extension_id_)
    return true;

  bool allowed_with_active_tab =
      origin_url.SchemeIs(content::kChromeUIScheme) ||
      origin_url.SchemeIs(kExtensionScheme) ||
      document_url.SchemeIs(url::kDataScheme) ||
      origin.IsSameOriginWith(
          url::Origin::Create(ExtensionsClient::Get()->GetWebstoreBaseURL()));

  if (!allowed_with_active_tab) {
    if (error)
       *error = access_error;
     return false;
   }
   if (has_active_tab)
     return true;

  if (error)
    *error = manifest_errors::kActiveTabPermissionNotGranted;
  return false;
}
