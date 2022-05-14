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
 
    // Check if any of the host permissions match all urls. We don't use
    // URLPatternSet::ContainsPattern() here because a) the schemes may be
    // different and b) this is more efficient.
    for (const auto& pattern : active_permissions_unsafe_->explicit_hosts()) {
      if (pattern.match_all_urls()) {
        has_all_urls = true;
        break;
      }
    }

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

    if (GetPageAccess(origin_url, tab_id, &access_error) ==
        PageAccess::kAllowed)
      return true;
  } else {
    DCHECK_EQ(CaptureRequirement::kPageCapture, capture_requirement);
    if (!has_page_capture) {
      if (error)
        *error = manifest_errors::kPageCaptureNeeded;
    }

    if ((origin_url.SchemeIs(url::kHttpScheme) ||
         origin_url.SchemeIs(url::kHttpsScheme)) &&
        !origin.IsSameOriginWith(url::Origin::Create(
            ExtensionsClient::Get()->GetWebstoreBaseURL()))) {
      return true;
    }
  }


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
