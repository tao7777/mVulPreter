bool IsSensitiveURL(const GURL& url,
                    bool is_request_from_browser_or_webui_renderer) {
  bool sensitive_chrome_url = false;
  const char kGoogleCom[] = "google.com";
  const char kClient[] = "clients";
  url::Origin origin = url::Origin::Create(url);
  if (origin.DomainIs(kGoogleCom)) {
    base::StringPiece host = url.host_piece();
    while (host.ends_with("."))
      host.remove_suffix(1u);
    if (is_request_from_browser_or_webui_renderer) {
      base::StringPiece::size_type pos = host.rfind(kClient);
      if (pos != base::StringPiece::npos) {
        bool match = true;
        if (pos > 0 && host[pos - 1] != '.') {
          match = false;
        } else {
          for (base::StringPiece::const_iterator
                   i = host.begin() + pos + strlen(kClient),
                   end = host.end() - (strlen(kGoogleCom) + 1);
               i != end; ++i) {
            if (!isdigit(*i)) {
              match = false;
              break;
            }
          }
        }
        sensitive_chrome_url = sensitive_chrome_url || match;
      }
    }

    sensitive_chrome_url = sensitive_chrome_url ||
                           (url.DomainIs("chrome.google.com") &&
                             base::StartsWith(url.path_piece(), "/webstore",
                                              base::CompareCase::SENSITIVE));
   }
   return sensitive_chrome_url || extension_urls::IsWebstoreUpdateUrl(url) ||
          extension_urls::IsBlacklistUpdateUrl(url) ||
          extension_urls::IsSafeBrowsingUrl(origin, url.path_piece());
}
