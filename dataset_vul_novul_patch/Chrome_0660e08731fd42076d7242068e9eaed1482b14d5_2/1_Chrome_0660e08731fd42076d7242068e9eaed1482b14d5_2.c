  bool CanCapture(const Extension& extension, const GURL& url) {
     return extension.permissions_data()->CanCaptureVisiblePage(
        url, kTabId, nullptr /*error*/);
   }
