bool PermissionsData::CanRunOnPage(const Extension* extension,
                                   const GURL& document_url,
                                   const GURL& top_frame_url,
                                   int tab_id,
                                   int process_id,
                                   const URLPatternSet& permitted_url_patterns,
                                   std::string* error) const {
  if (g_policy_delegate &&
      !g_policy_delegate->CanExecuteScriptOnPage(
          extension, document_url, top_frame_url, tab_id, process_id, error)) {
     return false;
   }
 
  bool can_execute_everywhere = CanExecuteScriptEverywhere(extension);
  if (!can_execute_everywhere &&
      !ExtensionsClient::Get()->IsScriptableURL(document_url, error)) {
    return false;
  }
  if (!base::CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kExtensionsOnChromeURLs)) {
    if (document_url.SchemeIs(content::kChromeUIScheme) &&
        !can_execute_everywhere) {
      if (error)
        *error = manifest_errors::kCannotAccessChromeUrl;
      return false;
    }
  }
  if (top_frame_url.SchemeIs(kExtensionScheme) &&
      top_frame_url.GetOrigin() !=
          Extension::GetBaseURLFromExtensionId(extension->id()).GetOrigin() &&
      !can_execute_everywhere) {
    if (error)
      *error = manifest_errors::kCannotAccessExtensionUrl;
     return false;
  }
 
   if (HasTabSpecificPermissionToExecuteScript(tab_id, top_frame_url))
     return true;

  bool can_access = permitted_url_patterns.MatchesURL(document_url);

  if (!can_access && error) {
    *error = ErrorUtils::FormatErrorMessage(manifest_errors::kCannotAccessPage,
                                            document_url.spec());
  }

  return can_access;
}
