 void GenerateHostsToCheck(const GURL& url, std::vector<std::string>* hosts) {
   hosts->clear();

  std::string canon_host;
  CanonicalizeUrl(url, &canon_host, NULL, NULL);

  const std::string host = canon_host;  // const sidesteps GCC bugs below!
   if (host.empty())
     return;
 
  const size_t kMaxHostsToCheck = 4;
  bool skipped_last_component = false;
  for (std::string::const_reverse_iterator i(host.rbegin());
       i != host.rend() && hosts->size() < kMaxHostsToCheck; ++i) {
    if (*i == '.') {
      if (skipped_last_component)
        hosts->push_back(std::string(i.base(), host.end()));
      else
        skipped_last_component = true;
    }
  }
  hosts->push_back(host);
}
