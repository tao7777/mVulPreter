 bool TranslateManager::IsTranslatableURL(const GURL& url) {
  return !url.SchemeIs("chrome") && !url.SchemeIs("ftp");
 }
