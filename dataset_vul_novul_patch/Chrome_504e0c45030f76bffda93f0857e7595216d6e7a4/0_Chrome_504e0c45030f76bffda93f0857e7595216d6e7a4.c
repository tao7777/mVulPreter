std::set<std::string> GetDistinctHosts(const URLPatternSet& host_patterns,
                                       bool include_rcd,
                                       bool exclude_file_scheme) {
  typedef base::StringPairs HostVector;
  HostVector hosts_best_rcd;
  for (const URLPattern& pattern : host_patterns) {
    if (exclude_file_scheme && pattern.scheme() == url::kFileScheme)
       continue;
 
     std::string host = pattern.host();
    if (!host.empty()) {
      // Convert the host into a secure format. For example, an IDN domain is
      // converted to punycode.
      host = base::UTF16ToUTF8(url_formatter::FormatUrlForSecurityDisplay(
          GURL(base::StringPrintf("%s%s%s", url::kHttpScheme,
                                  url::kStandardSchemeSeparator, host.c_str())),
          url_formatter::SchemeDisplay::OMIT_HTTP_AND_HTTPS));
    }
 
     if (pattern.match_subdomains())
      host = "*." + host;


    std::string rcd;
    size_t reg_len =
        net::registry_controlled_domains::PermissiveGetHostRegistryLength(
            host, net::registry_controlled_domains::EXCLUDE_UNKNOWN_REGISTRIES,
            net::registry_controlled_domains::EXCLUDE_PRIVATE_REGISTRIES);
    if (reg_len && reg_len != std::string::npos) {
      if (include_rcd)  // else leave rcd empty
        rcd = host.substr(host.size() - reg_len);
      host = host.substr(0, host.size() - reg_len);
    }

    HostVector::iterator it = hosts_best_rcd.begin();
    for (; it != hosts_best_rcd.end(); ++it) {
      if (it->first == host)
        break;
    }
    if (it != hosts_best_rcd.end()) {
      if (include_rcd && RcdBetterThan(rcd, it->second))
        it->second = rcd;
    } else {  // Previously unseen host, append it.
      hosts_best_rcd.push_back(std::make_pair(host, rcd));
    }
  }

  std::set<std::string> distinct_hosts;
  for (const auto& host_rcd : hosts_best_rcd)
    distinct_hosts.insert(host_rcd.first + host_rcd.second);
  return distinct_hosts;
}
