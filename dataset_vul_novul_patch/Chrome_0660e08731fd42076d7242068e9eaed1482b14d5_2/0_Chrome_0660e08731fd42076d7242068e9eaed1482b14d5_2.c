  bool CanCapture(const Extension& extension, const GURL& url) {
  bool CanCapture(const Extension& extension,
                  const GURL& url,
                  extensions::CaptureRequirement capture_requirement) {
     return extension.permissions_data()->CanCaptureVisiblePage(
        url, kTabId, nullptr /*error*/, capture_requirement);
   }
