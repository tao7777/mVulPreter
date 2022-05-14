bool ChromeWebUIControllerFactory::HasWebUIScheme(const GURL& url) const {
  return url.SchemeIs(chrome::kChromeDevToolsScheme) ||
         url.SchemeIs(chrome::kChromeInternalScheme) ||
         url.SchemeIs(chrome::kChromeUIScheme);
}
