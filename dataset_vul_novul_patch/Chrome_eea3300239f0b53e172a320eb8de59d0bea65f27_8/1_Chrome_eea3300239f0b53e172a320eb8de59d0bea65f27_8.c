GURL SanitizeFrontendURL(
    const GURL& url,
    const std::string& scheme,
    const std::string& host,
    const std::string& path,
    bool allow_query) {
  std::vector<std::string> query_parts;
  if (allow_query) {
    for (net::QueryIterator it(url); !it.IsAtEnd(); it.Advance()) {
      std::string value = SanitizeFrontendQueryParam(it.GetKey(),
          it.GetValue());
      if (!value.empty()) {
        query_parts.push_back(
            base::StringPrintf("%s=%s", it.GetKey().c_str(), value.c_str()));
      }
    }
  }
  std::string query =
      query_parts.empty() ? "" : "?" + base::JoinString(query_parts, "&");
  std::string constructed = base::StringPrintf("%s://%s%s%s",
      scheme.c_str(), host.c_str(), path.c_str(), query.c_str());
  GURL result = GURL(constructed);
  if (!result.is_valid())
    return GURL();
  return result;
}
