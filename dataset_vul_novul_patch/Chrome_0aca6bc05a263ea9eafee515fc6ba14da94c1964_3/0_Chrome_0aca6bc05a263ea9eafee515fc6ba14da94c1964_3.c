  AccessType GetExtensionAccess(const Extension* extension,
                                 const GURL& url,
                                 int tab_id) {
     bool allowed_script = IsAllowedScript(extension, url, tab_id);
    bool allowed_capture = extension->permissions_data()->CanCaptureVisiblePage(
        url, extension, tab_id, nullptr);
 
     if (allowed_script && allowed_capture)
       return ALLOWED_SCRIPT_AND_CAPTURE;
    if (allowed_script)
      return ALLOWED_SCRIPT_ONLY;
    if (allowed_capture)
      return ALLOWED_CAPTURE_ONLY;
    return DISALLOWED;
  }
