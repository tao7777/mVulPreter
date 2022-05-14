  bool IsAllowed(const scoped_refptr<const Extension>& extension,
                 const GURL& url,
                 PermittedFeature feature,
                 int tab_id) {
    const PermissionsData* permissions_data = extension->permissions_data();
    bool script = permissions_data->CanAccessPage(extension.get(), url, tab_id,
                                                   nullptr) &&
                   permissions_data->CanRunContentScriptOnPage(
                       extension.get(), url, tab_id, nullptr);
    bool capture = HasTabsPermission(extension, tab_id) &&
                   permissions_data->CanCaptureVisiblePage(tab_id, NULL);
     switch (feature) {
       case PERMITTED_SCRIPT_ONLY:
         return script && !capture;
      case PERMITTED_CAPTURE_ONLY:
        return capture && !script;
      case PERMITTED_BOTH:
        return script && capture;
      case PERMITTED_NONE:
        return !script && !capture;
    }
    NOTREACHED();
    return false;
  }
