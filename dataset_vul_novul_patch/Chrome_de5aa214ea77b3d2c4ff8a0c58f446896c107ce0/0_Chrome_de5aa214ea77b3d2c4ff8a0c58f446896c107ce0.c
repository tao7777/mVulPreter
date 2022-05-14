 bool TranslateManager::IsTranslatableURL(const GURL& url) {
  // A URLs is translatable unless it is one of the following:
  // - an internal URL (chrome:// and others)
  // - the devtools (which is considered UI)
  // - an FTP page (as FTP pages tend to have long lists of filenames that may
  //   confuse the CLD)
  return !url.SchemeIs(chrome::kChromeUIScheme) &&
         !url.SchemeIs(chrome::kChromeDevToolsScheme) &&
         !url.SchemeIs(chrome::kFtpScheme);
 }
