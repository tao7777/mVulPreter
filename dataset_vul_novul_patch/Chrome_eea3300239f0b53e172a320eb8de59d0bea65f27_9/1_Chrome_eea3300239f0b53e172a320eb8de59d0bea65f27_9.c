GURL DevToolsUI::SanitizeFrontendURL(const GURL& url) {
  return ::SanitizeFrontendURL(url, content::kChromeDevToolsScheme,
      chrome::kChromeUIDevToolsHost, SanitizeFrontendPath(url.path()), true);
}
