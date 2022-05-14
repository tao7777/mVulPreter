 void HttpAuthFilterWhitelist::SetWhitelist(
     const std::string& server_whitelist) {
  // TODO(eroman): Is this necessary? The issue is that
  // HttpAuthFilterWhitelist is trying to use ProxyBypassRules as a generic
  // URL filter. However internally it has some implicit rules for localhost
  // and linklocal addresses.
  rules_.ParseFromString(ProxyBypassRules::GetRulesToSubtractImplicit() + ";" +
                         server_whitelist);
 }
